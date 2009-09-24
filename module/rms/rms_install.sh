echo "start rms tools installation"
echo "copy ini files"
cp *.ini $RATTREE/etc
echo "copy mesh ribgen to $RATTREE/lib/plugins"
cp ../ribgen/meshRibgen.* $RATTREE/lib/plugins
echo "copy mesh procedural to $RATTREE/lib/plugins";
cp ../procedural/meshCacheProcedural.* $RATTREE/lib/plugins
echo "copy prt rsl plugin to $RMANTREE/lib/shaders";
cp zPRT.so $RMANTREE/lib/shaders
echo "copy prt rsl plugin to $RATTREE/lib/slim/include";
cp zPRT.so $RATTREE/lib/slim/include
echo "copy slim templates to $RATTREE/lib/slim";
cp z_colors_prt.slim $RATTREE/lib/slim
cp ../slim/z_lights_hdr.slim $RATTREE/lib/slim
cp ../slim/displacement_z.slim $RATTREE/lib/slim
cp ../slim/fur.slim $RATTREE/lib/slim
cp ../slim/eye.slim $RATTREE/lib/slim
cp ../slim/ace.slim $RATTREE/lib/slim
cp ../slim/light_z.slim $RATTREE/lib/slim
echo "end rms tools installation"
