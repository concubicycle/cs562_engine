//
// Created by sava on 1/25/20.
//

#ifndef WIZARDENGINE_MODEL_LOADER_HPP
#define WIZARDENGINE_MODEL_LOADER_HPP

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace asset
{

    class model_loader
    {
    public:
        // todo: return type for this should be not void, but some model type.
        void load(std::string path);
    };

}

#endif //WIZARDENGINE_MODEL_LOADER_HPP
