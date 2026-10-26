#!/usr/bin/env python3
"""plutil / PlistBuddy stand-in for linux.

  plist.py get <plist> <key>                 print a top-level scalar
  plist.py has <plist> <key>                  exit 0 if the key exists
  plist.py alt-icons <plist> <base> <name>... register CFBundleAlternateIcons

The Info.plist inside an ipa is a binary plist, so the format found on read is
the format written back - plistlib would otherwise turn it into xml.
"""

import plistlib
import sys


def load(path):
    with open(path, "rb") as f:
        data = f.read()
    return plistlib.loads(data), data[:8] == b"bplist00"


def save(path, obj, binary):
    with open(path, "wb") as f:
        plistlib.dump(obj, f,
                      fmt=plistlib.FMT_BINARY if binary else plistlib.FMT_XML,
                      sort_keys=False)


def main(argv):
    if len(argv) < 4:
        sys.stderr.write(__doc__)
        return 2

    cmd, path, key = argv[1], argv[2], argv[3]
    root, binary = load(path)

    if cmd == "get":
        if key not in root:
            return 1
        v = root[key]
        if isinstance(v, bool):
            print("true" if v else "false")
            return 0
        if isinstance(v, (bytes, bytearray, dict, list)):
            sys.stderr.write("%s is not a scalar\n" % key)
            return 1
        print(v)
        return 0

    if cmd == "has":
        return 0 if key in root else 1

    if cmd == "alt-icons":
        names = argv[4:]
        if not names:
            sys.stderr.write("no icon names given\n")
            return 2
        if not isinstance(root.get(key), dict):
            return 1
        root[key]["CFBundleAlternateIcons"] = {
            n: {"CFBundleIconFiles": [n], "UIPrerenderedIcon": False}
            for n in names
        }
        save(path, root, binary)
        print("registered %d icon(s) under %s" % (len(names), key))
        return 0

    sys.stderr.write("unknown command: %s\n" % cmd)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
