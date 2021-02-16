#ifndef __ASSIMP_SCENE_HPP_
#define __ASSIMP_SCENE_HPP_

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace asset
{
  struct assimp_scene_asset
  {
    Assimp::Importer importer;
    const aiScene* aiscene{ nullptr };
  };
}

#endif