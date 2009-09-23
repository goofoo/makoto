echo "start rms tools installation"
echo "copy ini files"
cp *.ini $RATTREE/etc
echo "copy mesh ribgen to $RATTREE/lib/plugins"
cp ../ribgen/meshRibgen.* $RATTREE/lib/plugins
echo "copy mesh procedural to $RATTREE/lib/plugins";
cp ../procedural/meshCacheProcedural.* $RATTREE/lib/plugins
echo "end rms tools installation"
