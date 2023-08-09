#include <cstdint>
#include <cstdio>
#include <fstream>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif


#include <boost/json/src.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
// #include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>

// #include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
// #include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

// #include <ios>

// #include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>

// #include <llvm/Support/CommandLine.h>


boost::json::value read_boost_json (std::string input_file_name) {
    std::cout << "read json boost" << "\n";

    std::ifstream input_file(input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '" << input_file_name << "'" << std::endl;
        assert(false && "Could not open input file!");
    }

    boost::json::stream_parser p;
    boost::json::error_code ec;
    while (!input_file.eof()) {
        char input_string[256];
        input_file.read(input_string, sizeof(input_string) - 1);
        input_string[input_file.gcount()] = '\0';
        p.write(input_string, ec);
        if (ec) {
            std::cerr << "JSON parsing of public input failed" << std::endl;
            return 1;
        }
    }
    p.finish(ec);
    if (ec) {
        std::cerr << "JSON parsing of public input failed" << std::endl;
        return 1;
    }
    boost::json::value input_json_value = p.release();
    if (!input_json_value.is_array()) {
        std::cerr << "Array of arguments is expected in JSON file" << std::endl;
        return 1;
    }

    for (std::size_t i = 0; i < input_json_value.as_array().size(); i++) {
        if (!input_json_value.as_array()[i].is_object()) {
            assert(false && "input must containt json objects");
        }
    }

    return input_json_value;
}

template<typename BlueprintFieldType>
typename BlueprintFieldType::value_type parse_native_scalar(const boost::json::value &json_value) {
    std::cout << "parse_native_scalar" << "\n";

    std::cout << json_value << "\n";

    const std::size_t buflen = 256;
    char buf[buflen];
    std::size_t numlen = 0;

    typename BlueprintFieldType::extended_integral_type integral_res;
    typename BlueprintFieldType::value_type res;

    switch (json_value.kind()) {
    case boost::json::kind::int64:
        res = json_value.as_int64();
        return res;
    case boost::json::kind::uint64:
        res = json_value.as_int64();
        return res;
    case boost::json::kind::string: {
        numlen = json_value.as_string().size();
        if (numlen > buflen - 1) {
            std::cerr << "json_value " << json_value.as_string() << " exceeds buffer size (" << buflen - 1 << ")\n";
            assert(false && "json_value size exceeds buffer size");
        }
        json_value.as_string().copy(buf, numlen);
        buf[numlen] = '\0';
        integral_res = typename BlueprintFieldType::extended_integral_type(buf);
        if (integral_res >= BlueprintFieldType::modulus) {
            assert(false && "Input does not fit into BlueprintFieldType");
        }
        res = integral_res;
        return res;
    }
    default:
        assert(false && "only integers and strings are supported");
    }
}

template<typename BlueprintFieldType>
std::vector<typename BlueprintFieldType::value_type> read_fields(std::string input_file_name) {
    std::cout << "read_fields" << "\n";


    boost::json::value input_json_value = read_boost_json (input_file_name);

    std::vector<typename BlueprintFieldType::value_type> res;

    for (std::size_t i = 0; i < input_json_value.as_array().size(); i++) {
        const boost::json::object &current_value = input_json_value.as_array()[i].as_object();
        if (current_value.size() != 1)
            assert(false && "field length must be 1");
        if(!current_value.contains("field"))
            assert(false && "json value must contain \"field\"");
        if (current_value.at("field").is_double()) {
            assert(false && "got double value for field argument. Probably the value is too big to be represented as integer. You can put it in quotes to avoid JSON parser restrictions.");
        }

        res.push_back(parse_native_scalar<BlueprintFieldType>(current_value.at("field")));
    }
    return res;
}