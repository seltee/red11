fxc.exe .\VertexShader.hlsl /Fo /Tvs_3_0
fxc.exe .\FragmentShader.hlsl /Fo /Tps_3_0
bin2header.exe .\VertexShader.vso
bin2header.exe .\FragmentShader.pso