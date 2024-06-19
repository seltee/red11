fxc.exe .\vertexUVShader.hlsl /Fo /Tvs_3_0
fxc.exe .\fragmentUVShader.hlsl /Fo /Tps_3_0
fxc.exe .\vertexColorShader.hlsl /Fo /Tvs_3_0
fxc.exe .\fragmentColorShader.hlsl /Fo /Tps_3_0
fxc.exe .\vertexUVSkinnedShader.hlsl /Fo /Tvs_3_0
fxc.exe .\fragmentUVSkinnedShader.hlsl /Fo /Tps_3_0
bin2header.exe .\vertexUVShader.vso
bin2header.exe .\fragmentUVShader.pso
bin2header.exe .\vertexUVSkinnedShader.vso
bin2header.exe .\fragmentUVSkinnedShader.pso
bin2header.exe .\vertexColorShader.vso
bin2header.exe .\fragmentColorShader.pso