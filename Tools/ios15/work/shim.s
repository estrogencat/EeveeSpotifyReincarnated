.macro RR name, target, reg, keep0
.globl \name
.p2align 2
\name:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  sub sp, sp, #256
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp q0, q1, [sp, #128]
  stp q2, q3, [sp, #160]
  stp q4, q5, [sp, #192]
  stp q6, q7, [sp, #224]
  mov x0, \reg
  bl \target
  .if \keep0
  ldp x0, x1, [sp, #0]
  .else
  ldr x1, [sp, #8]
  .endif
  ldp x2, x3, [sp, #16]
  ldp x4, x5, [sp, #32]
  ldp x6, x7, [sp, #48]
  ldp x8, x9, [sp, #64]
  ldp x10, x11, [sp, #80]
  ldp x12, x13, [sp, #96]
  ldp x14, x15, [sp, #112]
  ldp q0, q1, [sp, #128]
  ldp q2, q3, [sp, #160]
  ldp q4, q5, [sp, #192]
  ldp q6, q7, [sp, #224]
  add sp, sp, #256
  ldp x29, x30, [sp], #16
  ret
.endm
.text
RR _objc_retain_x1, _objc_retain, x1, 0
RR _objc_release_x1, _objc_release, x1, 1
RR _objc_retain_x2, _objc_retain, x2, 0
RR _objc_release_x2, _objc_release, x2, 1
RR _objc_retain_x3, _objc_retain, x3, 0
RR _objc_release_x3, _objc_release, x3, 1
RR _objc_retain_x4, _objc_retain, x4, 0
RR _objc_release_x4, _objc_release, x4, 1
RR _objc_retain_x5, _objc_retain, x5, 0
RR _objc_release_x5, _objc_release, x5, 1
RR _objc_retain_x6, _objc_retain, x6, 0
RR _objc_release_x6, _objc_release, x6, 1
RR _objc_retain_x7, _objc_retain, x7, 0
RR _objc_release_x7, _objc_release, x7, 1
RR _objc_retain_x8, _objc_retain, x8, 0
RR _objc_release_x8, _objc_release, x8, 1
RR _objc_retain_x9, _objc_retain, x9, 0
RR _objc_release_x9, _objc_release, x9, 1
RR _objc_retain_x10, _objc_retain, x10, 0
RR _objc_release_x10, _objc_release, x10, 1
RR _objc_retain_x11, _objc_retain, x11, 0
RR _objc_release_x11, _objc_release, x11, 1
RR _objc_retain_x12, _objc_retain, x12, 0
RR _objc_release_x12, _objc_release, x12, 1
RR _objc_retain_x13, _objc_retain, x13, 0
RR _objc_release_x13, _objc_release, x13, 1
RR _objc_retain_x14, _objc_retain, x14, 0
RR _objc_release_x14, _objc_release, x14, 1
RR _objc_retain_x15, _objc_retain, x15, 0
RR _objc_release_x15, _objc_release, x15, 1
RR _objc_retain_x19, _objc_retain, x19, 0
RR _objc_release_x19, _objc_release, x19, 1
RR _objc_retain_x20, _objc_retain, x20, 0
RR _objc_release_x20, _objc_release, x20, 1
RR _objc_retain_x21, _objc_retain, x21, 0
RR _objc_release_x21, _objc_release, x21, 1
RR _objc_retain_x22, _objc_retain, x22, 0
RR _objc_release_x22, _objc_release, x22, 1
RR _objc_retain_x23, _objc_retain, x23, 0
RR _objc_release_x23, _objc_release, x23, 1
RR _objc_retain_x24, _objc_retain, x24, 0
RR _objc_release_x24, _objc_release, x24, 1
RR _objc_retain_x25, _objc_retain, x25, 0
RR _objc_release_x25, _objc_release, x25, 1
RR _objc_retain_x26, _objc_retain, x26, 0
RR _objc_release_x26, _objc_release, x26, 1
RR _objc_retain_x27, _objc_retain, x27, 0
RR _objc_release_x27, _objc_release, x27, 1
RR _objc_retain_x28, _objc_retain, x28, 0
RR _objc_release_x28, _objc_release, x28, 1
.globl _objc_claimAutoreleasedReturnValue
.p2align 2
_objc_claimAutoreleasedReturnValue:
  b _objc_retainAutoreleasedReturnValue
.globl _$s10Foundation3URLV9appending4path13directoryHintACx_AC09DirectoryF0OtSyRzlF
.p2align 2
_$s10Foundation3URLV9appending4path13directoryHintACx_AC09DirectoryF0OtSyRzlF:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  mov x4, x2
  mov x1, x0
  mov x0, x8
  mov x2, x20
  mov x3, x4
  bl _shim_url_appending
  ldp x29, x30, [sp], #16
  ret
.globl _shim_call_appendpc
.p2align 2
_shim_call_appendpc:
  stp x29, x30, [sp, #-32]!
  mov x29, sp
  str x20, [sp, #16]
  mov x8, x0
  mov x6, x1
  mov x0, x2
  mov x1, x3
  mov x2, x4
  mov x20, x6
  blr x5
  ldr x20, [sp, #16]
  ldp x29, x30, [sp], #32
  ret
.globl _$s10Foundation3URLV9appending10queryItemsACSayAA12URLQueryItemVG_tF
.p2align 2
_$s10Foundation3URLV9appending10queryItemsACSayAA12URLQueryItemVG_tF:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  mov x1, x0
  mov x0, x8
  mov x2, x20
  bl _shim_url_queryitems
  ldp x29, x30, [sp], #16
  ret

.globl _$s10Foundation6LocaleV6regionAC6RegionVSgvg
.p2align 2
_$s10Foundation6LocaleV6regionAC6RegionVSgvg:
  mov x0, x8
  b _shim_locale_region

.globl _shim_call_self0
.p2align 2
_shim_call_self0:
  stp x29, x30, [sp, #-32]!
  mov x29, sp
  str x20, [sp, #16]
  mov x9, x1
  mov x20, x0
  blr x9
  ldr x20, [sp, #16]
  ldp x29, x30, [sp], #32
  ret

.globl _shim_call_unbridge
.p2align 2
_shim_call_unbridge:
  stp x29, x30, [sp, #-16]!
  mov x29, sp
  mov x9, x2
  mov x8, x0
  mov x0, x1
  blr x9
  ldp x29, x30, [sp], #16
  ret

.globl _shim_call_mk
.p2align 2
_shim_call_mk:
  stp x29, x30, [sp, #-32]!
  mov x29, sp
  str x20, [sp, #16]
  mov x9, x2
  mov x8, x0
  mov x20, x1
  blr x9
  ldr x20, [sp, #16]
  ldp x29, x30, [sp], #32
  ret

.globl _shim_call_hcinit
.p2align 2
_shim_call_hcinit:
  stp x29, x30, [sp, #-32]!
  mov x29, sp
  str x20, [sp, #16]
  mov x9, x2
  mov x20, x1
  blr x9
  ldr x20, [sp, #16]
  ldp x29, x30, [sp], #32
  ret

.globl _swift_getKeyPath
.p2align 2
_swift_getKeyPath:
  b _shim_getkeypath

.globl _shim_call_subinit
.p2align 2
_shim_call_subinit:
  stp x29, x30, [sp, #-32]!
  mov x29, sp
  str x0, [sp, #16]
  mov x0, x1
  mov x1, x2
  blr x3
  ldr x4, [sp, #16]
  stp x0, x1, [x4]
  stp x2, x3, [x4, #16]
  ldp x29, x30, [sp], #32
  ret

.globl _$s17_StringProcessing5RegexV5MatchV13dynamicMemberqd__s7KeyPathCyxqd__G_tcluig
.p2align 2
_$s17_StringProcessing5RegexV5MatchV13dynamicMemberqd__s7KeyPathCyxqd__G_tcluig:
  mov x0, x8
  b _shim_regex_dynmember

.globl _$ss8DurationV10FoundationE9formattedy12FormatOutputQzxAC0D5StyleRzAB0D5InputRtzlF
.p2align 2
_$ss8DurationV10FoundationE9formattedy12FormatOutputQzxAC0D5StyleRzAB0D5InputRtzlF:
  mov x5, x8
  b _shim_dur_formatted

.section __TEXT,__const
.p2align 2
Lshim_mod:
  .long 0x40
  .long 0
  .long Lnm_mod - .

.macro NTD name, nameloc, acc
.globl \name
.p2align 2
\name:
  .long 0x51
  .long Lshim_mod - .
  .long \nameloc - .
  .long \acc - .
  .long 0
  .long 0
  .long 0
.endm

NTD _$s10Foundation6LocaleV12LanguageCodeVMn, Lnm_langcode, _$s10Foundation6LocaleV12LanguageCodeVMa
NTD _$s10Foundation6LocaleV6RegionVMn, Lnm_region, _$s10Foundation6LocaleV6RegionVMa
NTD _$s10Foundation6LocaleV6ScriptVMn, Lnm_script, _$s10Foundation6LocaleV6ScriptVMa
NTD _$s10Foundation6LocaleV8CurrencyVMn, Lnm_currency, _$s10Foundation6LocaleV8CurrencyVMa
NTD _$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitVMn, Lnm_unit, _$ss8DurationV10FoundationE16UnitsFormatStyleV4UnitVMa
NTD _$s7SwiftUI32UIHostingControllerSizingOptionsVMn, Lnm_sizing, _$s7SwiftUI32UIHostingControllerSizingOptionsVMa

Lnm_mod:      .asciz "Shim"
Lnm_langcode: .asciz "LanguageCode"
Lnm_region:   .asciz "Region"
Lnm_script:   .asciz "Script"
Lnm_currency: .asciz "Currency"
Lnm_unit:     .asciz "Unit"
Lnm_sizing:   .asciz "UIHostingControllerSizingOptions"
Lnm_hostcfg:  .asciz "UIHostingConfiguration"
Lnm_contentview: .asciz "ShimContentView"

NTD _$s7SwiftUI22UIHostingConfigurationVMn, Lnm_hostcfg, _$s7SwiftUI22UIHostingConfigurationVMa

.p2align 2
Lshim_smod:
  .long 0x40
  .long 0
  .long Lnm_smod - .

.globl _$s1S1AVMn
.p2align 2
_$s1S1AVMn:
  .long 0xd1
  .long Lshim_smod - .
  .long Lnm_sha - .
  .long _$s1S1AVMa - .
  .long 0
  .long 0
  .long 0
  .long 0
  .long 0
  .short 1
  .short 0
  .short 1
  .short 0
  .byte 0x80
  .byte 0
  .byte 0
  .byte 0

Lnm_smod: .asciz "S"
Lnm_sha:  .asciz "A"

.p2align 2
Lshim_swiftui:
  .long 0x40
  .long 0
  .long Lnm_swiftui - .

.globl _$s7SwiftUI23_GeometryActionModifierVMn
.p2align 2
_$s7SwiftUI23_GeometryActionModifierVMn:
  .long 0xd1
  .long Lshim_swiftui - .
  .long Lnm_geoact - .
  .long _$s7SwiftUI23_GeometryActionModifierVMa - .
  .long 0
  .long 0
  .long 0
  .long 0
  .long 0
  .short 1
  .short 0
  .short 1
  .short 0
  .byte 0x80
  .byte 0
  .byte 0
  .byte 0

Lnm_swiftui: .asciz "SwiftUI"
Lnm_geoact:  .asciz "_GeometryActionModifier"

.section __TEXT,__swift5_types,regular,no_dead_strip
.p2align 2
.globl _shim_tyrec
_shim_tyrec:
  .long _$s1S1AVMn - .
.globl _shim_tyrec_geo
_shim_tyrec_geo:
  .long _$s7SwiftUI23_GeometryActionModifierVMn - .

.section __TEXT,__const

.section __DATA,__data
.p2align 3
.globl _shim_p_uicontentconfig
_shim_p_uicontentconfig:
  .quad _$s5UIKit22UIContentConfigurationMp
.globl _shim_p_uicontentview
_shim_p_uicontentview:
  .quad _$s5UIKit13UIContentViewMp

.globl _shim_wt_cfg
.p2align 3
_shim_wt_cfg:
  .quad _$s7SwiftUI22UIHostingConfigurationVyxq_G5UIKit09UIContentD0AAMc
  .quad 0, 0, 0, 0, 0, 0, 0, 0

.globl _shim_wt_view
.p2align 3
_shim_wt_view:
  .quad _shim_conf_contentview
  .quad 0, 0, 0, 0, 0, 0, 0, 0

.globl _shim_p_viewmodifier
.p2align 3
_shim_p_viewmodifier:
  .quad _$s7SwiftUI12ViewModifierMp

.section __TEXT,__const
.p2align 2
.globl _$s7SwiftUI23_GeometryActionModifierVyxGAA04ViewE0AAMc
_$s7SwiftUI23_GeometryActionModifierVyxGAA04ViewE0AAMc:
  .long (_shim_p_viewmodifier - .) + 1
  .long _$s7SwiftUI23_GeometryActionModifierVMn - .
  .long 0
  .long 0

.p2align 2
.globl _$s7SwiftUI23_GeometryActionModifierV5value6actionACyxGxAA0C5ProxyVc_yxctcfC
_$s7SwiftUI23_GeometryActionModifierV5value6actionACyxGxAA0C5ProxyVc_yxctcfC:
  mov x5, x8
  b _shim_geo_init

.p2align 2
.globl _$s7SwiftUI22UIHostingConfigurationVyxq_G5UIKit09UIContentD0AAMc
_$s7SwiftUI22UIHostingConfigurationVyxq_G5UIKit09UIContentD0AAMc:
  .long (_shim_p_uicontentconfig - .) + 1
  .long _$s7SwiftUI22UIHostingConfigurationVMn - .
  .long _shim_wt_cfg - .
  .long 0

.p2align 2
.globl _shim_conf_contentview
_shim_conf_contentview:
  .long (_shim_p_uicontentview - .) + 1
  .long Lnm_contentview - .
  .long _shim_wt_view - .
  .long 2

.p2align 2
.globl _$s7SwiftUI22UIHostingConfigurationVA2A9EmptyViewVRs_rlE7contentACyxAEGxyXE_tcfC
_$s7SwiftUI22UIHostingConfigurationVA2A9EmptyViewVRs_rlE7contentACyxAEGxyXE_tcfC:
  mov x4, x8
  b _shim_cfg_init

.p2align 2
.globl _shim_t_copyself_ret
.globl _$s7SwiftUI22UIHostingConfigurationV7marginsyACyxq_GAA4EdgeO3SetV_12CoreGraphics7CGFloatVtF
.globl _$s7SwiftUI22UIHostingConfigurationV10background7contentACyxqd__Gqd__yXE_tAA4ViewRd__lF
.globl _$s7SwiftUI22UIHostingConfigurationV10backgroundyACyxAA01_cD14BackgroundViewVyqd__GGqd__AA10ShapeStyleRd__lF
_shim_t_copyself_ret:
_$s7SwiftUI22UIHostingConfigurationV7marginsyACyxq_GAA4EdgeO3SetV_12CoreGraphics7CGFloatVtF:
_$s7SwiftUI22UIHostingConfigurationV10background7contentACyxqd__Gqd__yXE_tAA4ViewRd__lF:
_$s7SwiftUI22UIHostingConfigurationV10backgroundyACyxAA01_cD14BackgroundViewVyqd__GGqd__AA10ShapeStyleRd__lF:
  mov x0, x8
  mov x1, x20
  b _shim_cfg_copyself

.p2align 2
.globl _shim_t_makeview
.globl _$s7SwiftUI22UIHostingConfigurationV15makeContentView5UIKit09UIContentG0_So6UIViewCXcyF
_shim_t_makeview:
_$s7SwiftUI22UIHostingConfigurationV15makeContentView5UIKit09UIContentG0_So6UIViewCXcyF:
  mov x0, x20
  b _shim_cfg_makeview_c

.p2align 2
.globl _shim_t_view_get
_shim_t_view_get:
  mov x0, x8
  mov x1, x20
  b _shim_view_cfg_get

.p2align 2
.globl _shim_t_view_set
_shim_t_view_set:
  mov x1, x20
  b _shim_view_cfg_set

.p2align 2
.globl _shim_t_sh_state
_shim_t_sh_state:
  mov x0, x8
  mov x1, x20
  b _shim_sh_state_get
