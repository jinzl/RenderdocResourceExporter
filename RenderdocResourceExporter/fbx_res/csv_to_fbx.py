'''
CSV 文件导出成 FBX 文件
'''

import os
import sys
from pathlib import Path

current_folder = Path(__file__).absolute().parent
father_folder = str(current_folder.parent)
sys.path.append(father_folder)

from qt_utilities.setting_utility import SettingUtility
from fbx_res.fbx_export_option_dialog_const_mapper import FbxExportOptionDialogConstMapper as FCM


class CsvToFbx:

    emgr = None

    def __init__(self, emgr_):
        self.emgr = emgr_
        pass

    def get_arg(self, config_, key_):
        if config_.get(key_):
            return True
        else:
            return False
        pass

    def execute(self, csv_path_, fbx_path_, export_config_):
        
        # 先把旧的删掉
        if os.path.exists(fbx_path_):
            os.remove(fbx_path_)

        # 读取设置
        export_normal = self.get_arg(export_config_, FCM.c_export_normal)
        export_tangent = self.get_arg(export_config_, FCM.c_export_tangent)
        export_uv = self.get_arg(export_config_, FCM.c_export_uv)
        export_uv2 = self.get_arg(export_config_, FCM.c_export_uv2)
        export_uv3 = self.get_arg(export_config_, FCM.c_export_uv3)
        export_color = self.get_arg(export_config_, FCM.c_export_color)
        
        # 拼接bat路径
        current_folder = Path(__file__).absolute().parent
        parent_folder = str(current_folder)
        parent_folder = parent_folder.replace("\n","").replace("\r","").replace("\\","/")
        bat_path = parent_folder + "/csv_to_fbx.bat"

        # 参数列表
        args = []
        # bat / exe
        args.append(f'{bat_path}')
        # csv 路径
        args.append(f'"{csv_path_}"')
        # 可选 flags
        if export_normal:
            args.append("-normal")
        if export_tangent:
            args.append("-tangent")
        if export_uv:
            args.append("-uv")
        if export_uv2:
            args.append("-uv2")
        if export_uv3:
            args.append("-uv3")
        if export_color:
            args.append("-color")

        # 最终命令
        cmd = " ".join(args)

        # 调用（示例）
        result = os.system(cmd)

        if result != 0:
            self.emgr.ErrorDialog(
                f"csv_to_fbx.bat failed\n\n"
                f"Return code: {result}\n\n"
                f"Command:\n{cmd}"
            )

        pass

