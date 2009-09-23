echo "start rms tools installation"
echo "copy ini files"
cp *.ini $RATTREE/etc
echo "copy mesh ribgen to $RATTREE/lib/plugins"
cp ../ribgen/meshRibgen.* $RATTREE/lib/plugins
echo "copy mesh procedural to $RATTREE/lib/plugins";
cp ../procedural/meshCacheProcedural.* $RATTREE/lib/plugins
echo "copy prt rsl plugin to $RMANTREE/lib/shaders";
cp ../rslPlugin/zPRT.* $RMANTREE/lib/shaders
echo "end rms tools installation"
