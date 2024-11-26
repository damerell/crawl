/**
 * @file
 * @brief Acquirement and Trog/Oka/Sif gifts.
**/

#pragma once


#include "item-prop-enum.h"

bool acquirement_menu(bool with_ihpix = false, bool superior = false);

int acquirement_create_item(object_class_type class_wanted, int agent,
                            bool quiet, const coord_def &pos = coord_def(), bool debug = false);
