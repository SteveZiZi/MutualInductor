#ifndef COMMON_H
#define COMMON_H


#define  TOP_BAR_L      0
#define  TOP_BAR_T      0
#define  TOP_BAR_W      800
#define  TOP_BAR_H      30

#define  MENU_BAR_L     695
#define  MENU_BAR_T    (TOP_BAR_H+5)
#define  MENU_BAR_W    (800-MENU_BAR_L)
#define  MENU_BAR_H    (480-MENU_BAR_T)

#define  FUNC_PAGE_L    0
#define  FUNC_PAGE_T    0
#define  FUNC_PAGE_W   (800-MENU_BAR_W)
#define  FUNC_PAGE_H   (480-FUNC_PAGE_T)


/*****************************************
 *  参数设置页面
 *****************************************/
//互感器类型
#define  MUTUAL_TYPE_SHOW_L     10
#define  MUTUAL_TYPE_SHOW_T     15
#define  MUTUAL_TYPE_SHOW_W     150
#define  MUTUAL_TYPE_SHOW_H     50
//测试项目
#define  ITEM_TYPE_SHOW_L      (MUTUAL_TYPE_SHOW_L+MUTUAL_TYPE_SHOW_W+40)
#define  ITEM_TYPE_SHOW_T      (MUTUAL_TYPE_SHOW_T)
#define  ITEM_TYPE_SHOW_W       400
#define  ITEM_TYPE_SHOW_H      (MUTUAL_TYPE_SHOW_H)
//基本参数
#define  BASE_PARAM_WIDGET_L    10
#define  BASE_PARAM_WIDGET_T   (ITEM_TYPE_SHOW_T+MUTUAL_TYPE_SHOW_H+20)
#define  BASE_PARAM_WIDGET_W    380
#define  BASE_PARAM_WIDGET_H    325
//扩展参数
#define  EXTEND_PARAM_WIDGET_L (BASE_PARAM_WIDGET_L+BASE_PARAM_WIDGET_W+20)
#define  EXTEND_PARAM_WIDGET_T  BASE_PARAM_WIDGET_T
#define  EXTEND_PARAM_WIDGET_W (FUNC_PAGE_W-EXTEND_PARAM_WIDGET_L-10)
#define  EXTEND_PARAM_WIDGET_H  BASE_PARAM_WIDGET_H
//底部任务栏
#define  BOTTOM_BAR_L           10
#define  BOTTOM_BAR_T          (BASE_PARAM_WIDGET_T+BASE_PARAM_WIDGET_H+10)
#define  BOTTOM_BAR_W           FUNC_PAGE_W
#define  BOTTOM_BAR_H           30
//Stack窗口
#define STACK_WIDGET_SHOW_L     0
#define STACK_WIDGET_SHOW_T     0
#define STACK_WIDGET_SHOW_W     690
#define STACK_WIDGET_SHOW_H     430
//励磁数据中Stack窗口
#define STACK_WIDGET_W          690
#define STACK_WIDGET_H          430

// 参数配置按钮列表
enum
{
    START_BUTTON = 0,
    STOP_BUTTON,
    IMPORT_BUTTON,
    EXPORT_BUTTON
};

#endif // COMMON_H

