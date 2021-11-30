/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <fstream>
#include "PluginsConfig.h"
#include "PluginsManager.h"
#include "Json.h"
#include "NoContainer.h"

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    PluginsConfig::PluginsConfig(PluginsManager* pm) : manager(pm)
    {
    }

    //---------------------------------------------------------------------------
    PluginsConfig::~PluginsConfig()
    {
    }

    //---------------------------------------------------------------------------
    int PluginsConfig::load_file(const std::string& filename, std::string& error)
    {
        Container *c;
#ifdef HAVE_JANSSON
        c = new Json;
#else
        c = new NoContainer;
#endif

        std::ifstream file_handler(filename.c_str(), std::ios_base::ate);
        std::string buffer;

        if (!file_handler)
        {
            error = "Cannot open file: " + filename + ", use default";

            // Automatically load ISMC1 plugin by default, since its don't have external dependencies
            buffer = "{\"Plugins\":[{\"id\":\"ISMC1\",\"name\":\"IMSC1\",\"format\":\"TTML\"}]}";
        }
        else
        {
            buffer.reserve(file_handler.tellg());
            file_handler.seekg(0, file_handler.beg);
            buffer.assign(std::istreambuf_iterator<char>(file_handler),
                          std::istreambuf_iterator<char>());
            file_handler.close();
        }

        Container::Value values;
        if (c->parse(buffer.c_str(), values) < 0)
        {
            error = c->get_error();
            delete c;
            return -1;
        }

        Container::Value* value = c->get_value_by_key(values, "Plugins");
        if (!value || value->type != Container::Value::CONTAINER_TYPE_ARRAY)
        {
            error = std::string("No 'Plugins' array section in the plugins config file: ") + filename;
            delete c;
            return -1;
        }

        int ret = parse_struct(value->array, error);
        delete c;
        return ret;
    }

    //---------------------------------------------------------------------------
    int PluginsConfig::parse_struct(const std::vector<Container::Value>& conf, std::string& error)
    {
        for (size_t i = 0; i < conf.size(); ++i)
        {
            if (conf[i].type != Container::Value::CONTAINER_TYPE_OBJECT)
            {
                error += "Plugins element is not Object\n";
                continue;
            }

            if (conf[i].obj.find("name") == conf[i].obj.end() || conf[i].obj.at("name").type != Container::Value::CONTAINER_TYPE_STRING)
            {
                error += "Plugins element does not have the 'name' attribute.\n";
                continue;
            }

            manager->load_plugin(conf[i].obj, error);
        }

        return 0;
    }

}
