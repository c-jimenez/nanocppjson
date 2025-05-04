
#include <nanojsoncpp/nanojsoncpp.h>

namespace nanojsoncpp
{

/** @brief nanojsoncpp version in the form XX.YY.ZZ where:
 *          XX = Major version number (increase = API compatibility broken and/or major feature addition)
 *          YY = Minor version number (increase = Minor feature addition)
 *          ZZ = Fix version number (increase = Bug fix(es))
 *
 *          Version number is defined in the main CMakeLists.txt file of
 *          the build tree
 */
static const std::string_view NANOJSONCPP_VERSION = NANOJSONCPP_MAJOR "." NANOJSONCPP_MINOR "." NANOJSONCPP_FIX;

/** @brief Get the version string of the library */
const std::string_view& get_version()
{
    return NANOJSONCPP_VERSION;
}

} // namespace nanojsoncpp
