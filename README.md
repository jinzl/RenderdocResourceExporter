# RenderdocResourceExporter
The main feature is to export renderdoc captured mesh. <br>
Because I don't want to export csv and then convert it to fbx in other software.<br>
So I wrote this thing.<br>

Fix to new RDC version<br>
Add support to vertexCOlor<br>
Fixed an export issue where vertex indices did not start from zero<br>
Automatically detects vertex attributes present in the mesh<br>

C++ code modified from https://github.com/chineseoldghost/csv2fbx<br>

install：<br>

<img width="525" height="225" alt="ScreenShot_2026-01-18_230918_906" src="https://github.com/user-attachments/assets/04faf8c5-24c9-4b6e-b4e4-0f48ac724762" />

<img width="556" height="569" alt="ScreenShot_2026-01-18_230712_628" src="https://github.com/user-attachments/assets/9501de24-b17b-4c19-bee5-00d1b5cf3c45" />

use:<br>

<img width="759" height="314" alt="ScreenShot_2026-01-18_230638_324" src="https://github.com/user-attachments/assets/61172e4e-1f8e-4fb8-aaa8-cca96de28186" />
<img width="825" height="422" alt="sc_1" src="https://github.com/user-attachments/assets/31a17024-9e67-441e-969c-49800ef6ce08" />

---<br>

主要功能是导出renderdoc捕获的mesh。<br>
因为我不想先导出csv，然后到别的软件中去转换成fbx。<br>
所以我写了这个东西。<br>

修复了对新版RDC的支持<br>
添加了顶点颜色导出的支持<br>
修复了顶点序号不是从零开始的导出错误<br>
自动识别模型文件包含的顶点属性<br>

代码修改自 https://github.com/rrtt2323/RenderdocResourceExporter<br>

安装，使用见上图<br>
