cd "$(dirname "$0")" || exit 1
python gen_xj_patches.py || exit 1
python gen_shim.py || exit 1
python -m ziglang cc -target aarch64-macos.12.0 -mcpu=apple_a7 -dynamiclib -nostdlib -O1 \
  -o work/libshim.dylib work/shim.s work/gen.s work/impl.c work/tol.c \
  -Wl,-undefined,dynamic_lookup -Wl,-install_name,@rpath/libshim.dylib || exit 1
python fixplat.py || exit 1
python check_pac.py work/libshim.dylib || exit 1
python dump_shim_exports.py
