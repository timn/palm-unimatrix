/* $Id: resources.h,v 1.1 2003/02/07 01:07:52 tim Exp $
 *
 * ID's for use in ressource definitions
 *
 * Definitions:
 * 1:         Version
 * 100-199:   app specific values
 * 1001-1999: Forms, form items
 * 2001-2999: Menus and menu items
 * 4001-4999: Help
 * 8001-8999: Special stuff, bitmaps etc.
 */

#define APP_ID 100
#define APP_NAMEID 101
#define APP_CATEGORIES 102

#define FORM_main 1001
#define GADGET_main 1010
#define LIST_cat 1011
#define LIST_cat_popup 1012
#define LIST_cat_trigger 1013
#define STRING_cat_edit 1014
#define BUTTON_edit 1016
#define BUTTON_beam 1018
#define BUTTON_next 1002
#define GADGET_hint 1005

// free: 101x to 104x

#define FORM_courselist 1050
#define LIST_courses 1051
#define BUTTON_courselist_back 1052
#define BUTTON_courselist_edit 1053
#define BUTTON_courselist_del 1054
#define BUTTON_courselist_beam 1055
#define LIST_cl_cat 1056
#define LIST_cl_cat_popup 1057
#define LIST_cl_cat_trigger 1058
#define BUTTON_courselist_add 1059
#define STRING_edit_ctype 1060
#define FIELD_cl_website 1061
#define FIELD_cl_email 1062

#define FORM_about 1067
#define BITMAP_logo 1068
#define BUTTON_about_ok 1069

#define FORM_course_det 1070
#define BUTTON_cd_ok 1071
#define BUTTON_cd_cancel 1072
#define FIELD_cd_name 1073
#define FIELD_cd_teacher 1075
#define FIELD_cd_email 1076
#define FIELD_cd_website 1077
#define FIELD_cd_phone 1078
#define LIST_cd_type 1079
#define LIST_cd_type_trigger 1080

#define FORM_evt_det 1081
#define BUTTON_ed_ok 1082
#define BUTTON_ed_cancel 1083
#define LIST_ed_course 1084
#define LIST_ed_course_trigger 1085
#define LIST_ed_day 1086
#define LIST_ed_day_trigger 1087
#define SELECTOR_ed_time 1088
#define FIELD_ed_r 1089
#define FIELD_ed_g 1090
#define FIELD_ed_b 1091
#define FIELD_ed_room 1092
#define BUTTON_ed_del 1093
#define BUTTON_ed_beam 1094
#define BUTTON_ed_color 1095
#define STRING_ed_title 1096

#define FORM_beam_cat 1110
#define BUTTON_beam_cat_ok 1111
#define LIST_bc_cat 1112
#define LIST_bc_cat_trigger 1113
#define BUTTON_beam_cat_cancel 1114

#define FORM_settings 1120
#define CHECKBOX_sets_saturday 1121
#define BUTTON_sets_ok 1122
#define BUTTON_sets_cancel 1123
#define CHECKBOX_sets_showtype 1124
#define CHECKBOX_sets_showtime 1125

#define FORM_coursetypes 1130
#define LIST_coursetypes 1131
#define BUTTON_ct_back 1132
#define BUTTON_ct_add 1133
#define BUTTON_ct_edit 1134
#define BUTTON_ct_del 1135

#define FORM_ct_details 1140
#define FIELD_ctdet_name 1141
#define FIELD_ctdet_short 1142
#define BUTTON_ctdet_ok 1143
#define BUTTON_ctdet_cancel 1144

#define FORM_exams 1150
#define TABLE_exams 1151
#define BUTTON_ex_back 1152
#define BUTTON_ex_edit 1153
#define BUTTON_ex_del 1154
#define BUTTON_ex_beam 1155
#define LIST_ex_cat 1156
#define LIST_ex_cat_trigger 1158
#define BUTTON_ex_add 1159
#define REPEAT_ex_up 1160
#define REPEAT_ex_down 1161
#define REPEAT_ex_up_dis 1162
#define REPEAT_ex_down_dis 1163

#define FORM_exam_details 1170
#define LIST_exd_course 1171
#define LIST_exd_course_trigger 1172
#define SELECTOR_exd_date 1173
#define SELECTOR_exd_time 1174
#define BUTTON_exd_cancel 1175
#define BUTTON_exd_ok 1176
#define FIELD_exd_room 1177

#define FORM_beam_newcat 1180
#define FIELD_bs_cat 1181
#define BUTTON_bs_ok 1182
#define BUTTON_bs_cancel 1183

#define STRING_ctype_unknown 1149
#define STRING_ctype_short_unknown 1150
#define CTYPE_DEF_SH_START 1151
#define CTYPE_DEF_SH_LECTURE 1151
#define CTYPE_DEF_SH_EXERCISE 1152
#define CTYPE_DEF_SH_BIGEXERCISE 1153
#define CTYPE_DEF_SH_SEMINAR 1154
#define CTYPE_DEF_SH_PROSEMINAR 1155
#define CTYPE_DEF_SH_REPETITORIUM 1156

#define CTYPE_DEF_START 1161
#define CTYPE_DEF_LECTURE 1161
#define CTYPE_DEF_EXERCISE 1162
#define CTYPE_DEF_BIGEXERCISE 1163
#define CTYPE_DEF_SEMINAR 1164
#define CTYPE_DEF_PROSEMINAR 1165
#define CTYPE_DEF_REPETITORIUM 1166


#define MENU_main 2001
#define MENUITEM_course 2002
#define MENUITEM_time 2003
#define MENUITEM_about 2004
#define MENUITEM_courselist 2005
#define MENUITEM_settings 2006
#define MENUITEM_exams 2007
#define MENUITEM_beam 2008
#define MENUITEM_chat 2009
#define MENUITEM_mensa 2010
#define MENUITEM_addsep 2011

#define ALERT_ROMIncompatible 4001
#define ALERT_debug 4002
#define ALERT_nocourses 4003
#define ALERT_nomem 4004
#define ALERT_noroom 4005
#define ALERT_dodel 4006
#define ALERT_endBeforeBegin 4007
#define ALERT_invend 4008
#define ALERT_dodelc 4009
#define ALERT_clist_noitem 4010
#define ALERT_ec_noname 4011
#define ALERT_ec_noteacher 4012
#define ALERT_untimed_imp 4013
#define ALERT_oldDB 4014
#define ALERT_ctdet_inv 4015
#define ALERT_ct_dodel 4016
#define ALERT_ex_dodel 4017
#define ALERT_timeCollision 4018
#define ALERT_beamdbfail 4019
#define ALERT_beamnocatleft 4020


#define BITMAP_edit 5002
#define BITMAP_del 5003
#define BITMAP_del_clicked 5004
#define BITMAP_edit_clicked 5005
#define BITMAP_next 5006
#define BITMAP_next_clicked 5007
#define BITMAP_beam 5008
#define BITMAP_beam_clicked 5009
#define BITMAP_add 5010
#define BITMAP_add_clicked 5011
#define BITMAP_back 5012
#define BITMAP_back_clicked 5013
#define BITMAP_ok 5014
#define BITMAP_ok_clicked 5015
#define BITMAP_cancel 5016
#define BITMAP_cancel_clicked 5017


#define GADGET_STRINGS_MO 8111
#define GADGET_STRINGS_DI 8112
#define GADGET_STRINGS_MI 8113
#define GADGET_STRINGS_DO 8114
#define GADGET_STRINGS_FR 8115
#define GADGET_STRINGS_SA 8116
