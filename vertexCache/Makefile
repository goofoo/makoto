include ../shared/buildconfig

SRC = pluginMain.cpp cacheTransform.cpp importSceneCmd.cpp meshCacheCmd.cpp meshCacheNode.cpp meshCacheVizNode.cpp mshUtil.cpp obj2mcf.cpp prtAttribNode.cpp vxCacheDeformer.cpp xmlMeshCacheCmd.cpp ../shared/FColorSet.cpp ../shared/FFloatSet.cpp ../shared/FMCFMesh.cpp ../shared/FUVSet.cpp ../shared/FXMLMesh.cpp ../shared/FXMLScene.cpp ../shared/gBase.cpp ../shared/XMLUtil.cpp ../shared/zGlobal.cpp ../shared/zWorks.cpp ../shared/zXMLDoc.cpp ../sh_lighting/FQSPLAT.cpp ../sh_lighting/FTriangle.cpp ../sh_lighting/VisibilityBuffer.cpp

SRC1 = pluginMain.cpp meshCacheCmd.cpp ../shared/zWorks.cpp ../shared/FMCFMesh.cpp obj2mcf.cpp mshUtil.cpp meshCacheNode.cpp  xmlMeshCacheCmd.cpp vxCacheDeformer.cpp ../shared/FColorSet.cpp ../shared/FFloatSet.cpp ../shared/FUVSet.cpp ../shared/FXMLMesh.cpp ../shared/FXMLScene.cpp  ../shared/XMLUtil.cpp ../shared/zXMLDoc.cpp ../sh_lighting/FQSPLAT.cpp ../sh_lighting/FTriangle.cpp ../sh_lighting/VisibilityBuffer.cpp ../shared/gBase.cpp ../shared/zGlobal.cpp meshCacheVizNode.cpp prtAttribNode.cpp importSceneCmd.cpp cacheTransform.cpp 

all:
	$(LD) -o ../module/plug-ins/meshCache.so $(SRC1) $(INCLUDES) $(LIBS) -lOpenMayaUI -lOpenMayaAnim -lxml2
