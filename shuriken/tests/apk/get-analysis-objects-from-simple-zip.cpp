//--------------------------------------------------------------------*- C++ -*-
// Shuriken-Analyzer: library for bytecode analysis.
// @author Farenain <kunai.static.analysis@gmail.com>
//
// @file get-analysis-objects-from-simple-zip.cpp
// @brief Core API test to check if the get analysis classes and methods work

#include "dex-files-folder.inc"
#include "shuriken/api/C/shuriken_core.h"
#include "shuriken/api/C/shuriken_core_data.h"
#include <iostream>
#include <cassert>

int main() {
    std::string zip_file = std::string(DEX_FILES_FOLDER) + "test_zip.apk";

    hApkContext apk_context = parse_apk(zip_file.c_str(), TRUE);

    for (int i = 0,
             n_of_dex_files = get_number_of_dex_files(apk_context);
         i < n_of_dex_files;
         i++) {
        const char * dex_file = get_dex_file_by_index(apk_context, i);

        int number_of_strings = get_number_of_strings_from_dex(apk_context, dex_file);

        /**
         * Get the strings
         */
        for (int j = 0;
                 j < number_of_strings;
             j++) {
            [[maybe_unused]] auto * str = get_string_by_id_from_dex(apk_context, dex_file, j);

            std::cout << str << '\n';

            assert(str != nullptr && "Error retrieved a null string");

            [[maybe_unused]] hdvmstringanalysis_t * str_analysis = get_analyzed_string_from_apk(apk_context, str);

            if (str_analysis != nullptr) {
                assert(str_analysis->n_of_xreffrom > 0 && "The StringAnalysis must have xrefs");
            }
        }

        /**
         * Get the classes and methods by hdvmmethod_t
         */
        for (int j = 0,
                n_of_classes = get_number_of_classes_for_dex_file(apk_context, dex_file);
             j < n_of_classes;
             j++) {
            hdvmclass_t * cls = get_hdvmclass_from_dex_by_index(apk_context, dex_file, j);

            [[maybe_unused]] hdvmclassanalysis_t * cls_analysis = get_analyzed_class_by_hdvmclass_from_apk(apk_context, cls);

            assert(cls_analysis != nullptr && "Error get_analyzed_class_by_hdvmclass_from_apk didn't work");

            [[maybe_unused]] hdvmclassanalysis_t *cls_analysis2 = get_analyzed_class_from_apk(apk_context, cls->class_name);

            assert(cls_analysis2 != nullptr && "Error get_analyzed_class_from_apk didn't work");

            assert(cls_analysis == cls_analysis2 && "Error, returned two different hdvmclassanalysis_t");

            for (int z = 0,
                     n_of_virtual_methods = cls->virtual_methods_size;
                    z < n_of_virtual_methods;
                    z++) {
                hdvmmethod_t * method = &cls->virtual_methods[z];

                [[maybe_unused]] hdvmmethodanalysis_t * method_analysis = get_analyzed_method_by_hdvmmethod_from_apk(apk_context, method);

                assert(method_analysis != nullptr && "Error get_analyzed_method_by_hdvmmethod_from_apk didn't work");

                [[maybe_unused]] hdvmmethodanalysis_t * method_analysis2 = get_analyzed_method_from_apk(apk_context, method->dalvik_name);

                assert(method_analysis2 != nullptr && "Error get_analyzed_method_from_apk didn't work");

                assert(method_analysis == method_analysis2 && "Error, returned two different hdvmmethodanalysis_t");

                [[maybe_unused]] dvmdisassembled_method_t * disassembled_method = get_disassembled_method_from_apk(apk_context, method->dalvik_name);

                assert(disassembled_method != nullptr && "Error get_disassembled_method_from_apk didn't work");

            }

            for (int z = 0,
                     n_of_direct_methods = cls->direct_methods_size;
                    z < n_of_direct_methods;
                    z++) {
                hdvmmethod_t * method = &cls->direct_methods[z];

                [[maybe_unused]] hdvmmethodanalysis_t * method_analysis = get_analyzed_method_by_hdvmmethod_from_apk(apk_context, method);

                assert(method_analysis != nullptr && "Error get_analyzed_method_by_hdvmmethod_from_apk didn't work");

                [[maybe_unused]] hdvmmethodanalysis_t * method_analysis2 = get_analyzed_method_from_apk(apk_context, method->dalvik_name);

                assert(method_analysis2 != nullptr && "Error get_analyzed_method_from_apk didn't work");

                assert(method_analysis == method_analysis2 && "Error, returned two different hdvmmethodanalysis_t");

                [[maybe_unused]] dvmdisassembled_method_t * disassembled_method = get_disassembled_method_from_apk(apk_context, method->dalvik_name);

                assert(disassembled_method != nullptr && "Error get_disassembled_method_from_apk didn't work");
            }
        }
    }

    [[maybe_unused]] size_t num_of_method_analyses = get_number_of_methodanalysis_objects(apk_context);

    assert(num_of_method_analyses > 0 && "The call get_number_of_methodanalysis_objects didn't work");

    [[maybe_unused]] int num_of_externals = 0;

    for (size_t i = 0; i < num_of_method_analyses; i++) {
        hdvmmethodanalysis_t * m_analysis = get_analyzed_method_by_idx(apk_context, i);

        assert(m_analysis != nullptr && "Retrieved method analysis is null");

        if (m_analysis->external)
            num_of_externals++;
    }

    assert(num_of_externals > 0 && "Weird, there's not external methods...");

    destroy_apk(apk_context);
}