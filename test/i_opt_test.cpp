#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <iopt/image_optimizer.hpp>

TEST_CASE("Quick check", "[main]") {
    ImageOptimizer opt{};
    REQUIRE(opt.GetVersion().empty());
}
