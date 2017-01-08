# Software-Rendering
Software rendering implemented by C++ with out any DirectX or OpenGL API

一、定义基于左手坐标系的三维数据类型，包括：点（Vector）、变换矩阵（Transform Matrix）。注意点和矩阵的维数，选择不同的维数会需要后面实现不同的计算方法。
二、定义三维模型数据表示：顶点（Vertex）、Camera和三角形（Triangle）。
三、实现三维数据操作方法，包括：矢量的加（＋）、减（－）、点积（DotProduct）和插积（CrossProduct）；矢量和矩阵的乘积（点和向量的坐标变换）。
四、实现完整坐标变换算法，能够将三维空间中的某个点经过World Transform，View Transform，Project Transform后显示于屏幕之上。
五、实现三角形线框显示算法，能够将三角形的各个顶点经过坐标变换后变换到屏幕坐标，以连线的方式将三角形的各个边显示出来。不使用Windows API，而是利用2D Line算法，例如：2DDA算法。
六、实现基本的光照算法（Gouraud光照模式），支持环境光（Ambient Light）和一个直射光（Directional Light），能够根据当前光照信息和顶点的法向正确计算屏幕上相应点的颜色，并利用（四）中使用的2D Line算法来画出颜色渐变的三角形的各个边。并请实现支持由多个三角行组成的物体的显示。
七、实现Gouraud三角形着色算法，能够将（五）中的线框显示模式的三角形以Scanline方式进行填充，同时实现深度缓冲功能，最终实现支持由多个三角形构成的物体的显示。
八、实现Texture Mapping的Gouraud三角形着色算法，实现纹理映射的Perspective Correction。
九、实现剪裁算法，以避免当物体超出屏幕范围时出现错误。
十、实现Camera控制方法，可以在一定范围内调整Camera的位置和方向，以从不同方向观察由多个三角形组成的物体。