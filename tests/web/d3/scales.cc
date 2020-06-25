#include <iostream>
#include <vector>
#include <string>

#include "web/Document.h"
#include "web/d3/d3_init.h"
#include "d3_testing_utils.h"
#include "web/d3/scales.h"
#include "web/d3/utils.h"
#include "web/js_utils.h"

struct Test_Scales : BaseTest {

  void Setup() { ; }
  void Describe() {
    EM_ASM({
      describe("Continuous Scales", function() {
        it("should work", function() {
          chai.assert(false);
        });

        it("should match first values", function() ) {
          chai.assert(window["test_linear_scales"].js_results[0] === window["test_linear_scales"].cpp_results[0]);
        });

    });
  }

  ~Test_Scales() {
    EM_ASM({
      // window.linear_scale_test ??
      delete window["linear_scale_test"];
    })
  }
};

emp::web::Document doc("test_d3_init");
TestManager manager;

int main() {
  D3::internal::get_emp_d3();
  manager.AddTest<Test_Scales>();

  manager.OnBeforeEachTest([]() { ResetD3Context(); });
  manager.Run();

  return 0;
}