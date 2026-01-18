# RenderdocResourceExporter
The main feature is to export renderdoc captured mesh. 
Because I don't want to export csv and then convert it to fbx in other software.
So I wrote this thing.

Fix to new RDC version
Add support to vertexCOlor
Fixed an export issue where vertex indices did not start from zero
Automatically detects vertex attributes present in the mesh

C++ code modified from https://github.com/chineseoldghost/csv2fbx

install：

![image](https://user-images.githubusercontent.com/12709491/193525642-72e231b9-88b4-4d16-92ec-5802e83208d6.png)
![image](https://user-images.githubusercontent.com/12709491/193525544-19983e33-db7e-49ed-92c7-859f43e95e1e.png)

use:

![image](https://user-images.githubusercontent.com/12709491/193525856-08ad94b6-b19d-4ad1-a755-ee563e0e3b78.png)
<img width="825" height="422" alt="sc_1" src="https://github.com/user-attachments/assets/31a17024-9e67-441e-969c-49800ef6ce08" />

---

主要功能是导出renderdoc捕获的mesh。
因为我不想先导出csv，然后到别的软件中去转换成fbx。
所以我写了这个东西。

修复了对新版RDC的支持
添加了顶点颜色导出的支持
修复了顶点序号不是从零开始的导出错误
自动识别模型文件包含的顶点属性

代码修改自 https://github.com/rrtt2323/RenderdocResourceExporter

安装，使用见上图
