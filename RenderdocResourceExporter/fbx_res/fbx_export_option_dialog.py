'''
FBX 导出选项设置窗
'''

import os
import sys
from pathlib import Path
from functools import partial

from PySide2 import QtWidgets, QtCore, QtGui

current_folder = Path(__file__).absolute().parent
father_folder = str(current_folder.parent)
sys.path.append(father_folder)

from qt_utilities.setting_utility import SettingUtility
from qt_utilities.root_const_mapper import RootConstMapper as RCM
from fbx_res.fbx_export_option_dialog_const_mapper import FbxExportOptionDialogConstMapper as FCM


class FbxExportOptionDialog:

    emgr = None
    mqt = None

    default_config = [
        FCM.c_export_normal,
        FCM.c_export_tangent,
        FCM.c_export_binormal,
        FCM.c_export_uv,
        FCM.c_export_uv2,
        FCM.c_export_uv3,
        FCM.c_export_color,
    ]
    widget_dict = {}

    export_config = {}

    def __init__(self, emgr_, header_names):
        self.emgr = emgr_
        self.mqt = emgr_.GetMiniQtHelper()
        self.available_names = self.get_available_vertex_attributes(header_names)

        path, self.settings = SettingUtility.get_tempdir_setting(RCM.setting_preference)
        if not os.path.exists(path):
            self.refresh_option(0)
        pass

    def refresh_option(self, index_):
        if hasattr(self, "language"):
            language = self.language.itemText(index_)
        else:
            language = RCM.c_en
        self.settings.setValue(RCM.c_language, language)

        # 刷新ui显示
        if hasattr(self, "language_label"):
            self.mqt.SetWidgetText(self.language_label, FCM.get_text(FCM.c_language))

        for key, widget in self.widget_dict.items():
            self.mqt.SetWidgetText(widget.label, FCM.get_text(key))
        pass

    def init_ui(self):
        # 这里暂时没找到动态修改的方法，全靠每次重启了
        self.widget = self.mqt.CreateToplevelWidget(FCM.get_text(FCM.c_dialog_title), None)

        # 语言选项
        language_container = self.mqt.CreateHorizontalContainer()

        self.language_label = self.mqt.CreateLabel()
        self.mqt.SetWidgetText(self.language_label, FCM.get_text(FCM.c_language))

        self.language = QtWidgets.QComboBox()
        self.language.addItems([RCM.c_en, RCM.c_cn])
        self.language.setCurrentText(self.settings.value(RCM.c_language, RCM.c_en))
        self.language.currentIndexChanged.connect(self.refresh_option)

        self.mqt.AddWidget(language_container, self.language_label)
        self.mqt.AddWidget(language_container, self.language)
        self.mqt.AddWidget(self.widget, language_container)

        # 创建勾选项
        #for key in self.default_config:
        for key, has_attr in self.available_names.items():
            if not has_attr:
                continue
            widget = self.option_widget(key)
            self.mqt.AddWidget(self.widget, widget)
            self.widget_dict[key] = widget

            self.mqt.SetWidgetText(widget.label, FCM.get_text(key))
            self.mqt.SetWidgetChecked(widget.edit, True)

        # 按钮
        button_container = self.mqt.CreateHorizontalContainer()

        callback = lambda *args: self.mqt.CloseCurrentDialog(False)
        cancel_button = self.mqt.CreateButton(callback)
        self.mqt.SetWidgetText(cancel_button, FCM.get_text(FCM.c_cancel))

        ok_button = self.mqt.CreateButton(self.button_accept)
        self.mqt.SetWidgetText(ok_button, FCM.get_text(FCM.c_ok))

        self.mqt.AddWidget(button_container, cancel_button)
        self.mqt.AddWidget(button_container, ok_button)
        self.mqt.AddWidget(self.widget, button_container)

        return self.widget

    #选项的组件
    def option_widget(self, key_):
        container = self.mqt.CreateHorizontalContainer()

        label = self.mqt.CreateLabel()
        edit = self.mqt.CreateCheckbox(partial(self.option_change, key_))

        self.mqt.AddWidget(container, label)
        self.mqt.AddWidget(container, edit)

        container.label = label
        container.edit = edit

        return container

    #选项修改时
    def option_change(self, key_, context_, widget_, text_):
        pass

    #按钮确认
    def button_accept(self, context_, widget_, text_):
        for key, widget in self.widget_dict.items():
            value = self.mqt.IsWidgetChecked(widget.edit)
            self.export_config[key] = value

        self.mqt.CloseCurrentDialog(True)
        pass
        
    def normalize_vertex_attr_name(self, header_name_):
        """
        将 RenderDoc / DX / Vulkan / GLSL 风格的列名
        统一成“语义级”的顶点属性名
    
        规则：
        - POSITION / NORMAL / TANGENT / BINORMAL / COLOR ：统一去掉序号
        - TEXCOORD ：保留序号（TEXCOORD0 / TEXCOORD1 / TEXCOORD2）
        """
        if not header_name_:
            return ""
    
        name = str(header_name_).strip()
    
        # 1. 去掉 GLSL / Vulkan 的 in_ 前缀
        if name.startswith("in_"):
            name = name[3:]
    
        # 2. 去掉分量后缀 .x .y .z .w
        if "." in name:
            base, suffix = name.rsplit(".", 1)
            if suffix.lower() in ("x", "y", "z", "w"):
                name = base
    
        name = name.upper()
    
        # 3. 处理 TEXCOORD（唯一需要保留序号的语义）
        if name.startswith("TEXCOORD"):
            return name  # TEXCOORD0 / TEXCOORD1 / TEXCOORD2
    
        # 4. 其余语义统一去掉末尾数字（POSITION0 -> POSITION）
        #    只去末尾连续数字，避免误伤
        while name and name[-1].isdigit():
            name = name[:-1]
    
        return name
        
    def get_available_vertex_attributes(self, header_names_):
        """
        根据 QTableView 的列头名称，判断当前 mesh 实际拥有哪些顶点属性
    
        输入：
            header_names_ : List[str]
                例如：
                ["POSITION.x", "POSITION.y", "NORMAL.x", "TEXCOORD0.x", "COLOR0.w"]
    
        输出：
            Dict[key, bool]
                key 为 FCM.c_export_xxx
                value 表示该属性是否真实存在
        """
    
        # 先把所有列名统一成“语义级名字”
        semantic_names = set()
        for h in header_names_:
            semantic = self.normalize_vertex_attr_name(h)
            if semantic:
                semantic_names.add(semantic)
    
        # 根据语义判断属性是否存在
        available = {
            # 单实例语义
            FCM.c_export_normal:   "NORMAL"   in semantic_names,
            FCM.c_export_tangent:  "TANGENT"  in semantic_names,
            FCM.c_export_binormal: "BINORMAL" in semantic_names,
            FCM.c_export_color:    "COLOR"    in semantic_names,
    
            # TEXCOORD 是多通道语义
            FCM.c_export_uv:   "TEXCOORD0" in semantic_names,
            FCM.c_export_uv2:  "TEXCOORD1" in semantic_names,
            FCM.c_export_uv3:  "TEXCOORD2" in semantic_names,
        }
    
        return available

