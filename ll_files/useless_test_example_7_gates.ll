; ModuleID = '/root/cblpok/zkllvm/zkllvm/examples/useless_test_example_7_gates.cpp'
source_filename = "/root/cblpok/zkllvm/zkllvm/examples/useless_test_example_7_gates.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: circuit mustprogress nounwind uwtable
define dso_local noundef __zkllvm_field_pallas_base @_Z9field_addu26__zkllvm_field_pallas_baseu26__zkllvm_field_pallas_baseu28__zkllvm_field_pallas_scalaru30__zkllvm_field_curve25519_baseu30__zkllvm_field_curve25519_base(__zkllvm_field_pallas_base noundef %a_pallas_base, __zkllvm_field_pallas_base noundef %b_pallas_base, __zkllvm_field_pallas_scalar noundef %a_pallas_scalar, __zkllvm_curve_pallas %a_pallas_curve, __zkllvm_curve_pallas %b_pallas_curve, __zkllvm_field_curve25519_base noundef %a_25519_field, __zkllvm_field_curve25519_base noundef %b_25519_field) local_unnamed_addr #0 {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.inc
  %a_pallas_base.addr.018.lcssa = phi __zkllvm_field_pallas_base [ %a_pallas_base.addr.018, %for.inc ]
  %add.le = add __zkllvm_field_pallas_base %a_pallas_base.addr.018.lcssa, %b_pallas_base
  %sub.le = sub __zkllvm_field_pallas_base %add.le, %b_pallas_base
  %mul.le = mul __zkllvm_field_pallas_base %sub.le, %b_pallas_base
  br label %for.end

for.body:                                         ; preds = %entry, %for.inc
  %i.019 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %a_pallas_base.addr.018 = phi __zkllvm_field_pallas_base [ %a_pallas_base, %entry ], [ %mul, %for.inc ]
  %a_25519_field.addr.017 = phi __zkllvm_field_curve25519_base [ %a_25519_field, %entry ], [ %mul3, %for.inc ]
  %a_pallas_curve.addr.016 = phi __zkllvm_curve_pallas [ %a_pallas_curve, %entry ], [ %0, %for.inc ]
  %add = add __zkllvm_field_pallas_base %a_pallas_base.addr.018, %b_pallas_base
  %sub = sub __zkllvm_field_pallas_base %add, %b_pallas_base
  %mul = mul __zkllvm_field_pallas_base %sub, %b_pallas_base
  %add1 = add __zkllvm_field_curve25519_base %a_25519_field.addr.017, %b_25519_field
  %sub2 = sub __zkllvm_field_curve25519_base %add1, %b_25519_field
  %mul3 = mul __zkllvm_field_curve25519_base %sub2, %b_25519_field
  %0 = cmul __zkllvm_curve_pallas %a_pallas_curve.addr.016, __zkllvm_field_pallas_scalar %a_pallas_scalar
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %inc = add nsw i32 %i.019, 1
  %cmp = icmp slt i32 %inc, 10
  br i1 %cmp, label %for.body, label %for.cond.cleanup, !llvm.loop !5

for.end:                                          ; preds = %for.cond.cleanup
  %add4 = add __zkllvm_field_pallas_base %mul.le, %b_pallas_base
  ret __zkllvm_field_pallas_base %add4
}

attributes #0 = { circuit mustprogress nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.linker.options = !{}
!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 16.0.0 (git@github.com:NilFoundation/zkllvm-circifier.git 4d230ed398898e2328862fbde0e76a377d7d8884)"}
!5 = distinct !{!5, !6, !7}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.unroll.disable"}
