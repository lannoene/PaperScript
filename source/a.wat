(module(import "imports" "log" (func $printInt (param i32)))(func $add(param $a i32)(param $b i32)(result i32)(local.get $a)(local.get $b)(i32.add)(return))(func $start(export "start")(i32.const 1)(i32.const 1)(call $add)(call $printInt)))