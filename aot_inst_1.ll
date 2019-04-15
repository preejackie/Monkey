; ModuleID = '1.cpp'
source_filename = "1.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

$__llvm_profile_raw_version = comdat any

$__llvm_profile_filename = comdat any

@__llvm_profile_raw_version = constant i64 72057594037927940, comdat
@__profc__Z1hv = private global [1 x i64] zeroinitializer, section "__llvm_prf_cnts", align 8
@__profd__Z1hv = private global { i64, i64, i64*, i8*, i8*, i32, [2 x i16] } { i64 697383854498870107, i64 12884901887, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @__profc__Z1hv, i32 0, i32 0), i8* bitcast (void ()* @_Z1hv to i8*), i8* null, i32 1, [2 x i16] zeroinitializer }, section "__llvm_prf_data", align 8
@__profc_main = private global [2 x i64] zeroinitializer, section "__llvm_prf_cnts", align 8
@__profd_main = private global { i64, i64, i64*, i8*, i8*, i32, [2 x i16] } { i64 -2624081020897602054, i64 34137660316, i64* getelementptr inbounds ([2 x i64], [2 x i64]* @__profc_main, i32 0, i32 0), i8* bitcast (i32 ()* @main to i8*), i8* null, i32 2, [2 x i16] zeroinitializer }, section "__llvm_prf_data", align 8
@__llvm_prf_nm = private constant [12 x i8] c"\0A\00_Z1hv\01main", section "__llvm_prf_names", align 1
@llvm.used = appending global [3 x i8*] [i8* bitcast ({ i64, i64, i64*, i8*, i8*, i32, [2 x i16] }* @__profd__Z1hv to i8*), i8* bitcast ({ i64, i64, i64*, i8*, i8*, i32, [2 x i16] }* @__profd_main to i8*), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @__llvm_prf_nm, i32 0, i32 0)], section "llvm.metadata"
@__llvm_profile_filename = constant [19 x i8] c"default_%m.profraw\00", comdat

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @_Z1hv() #0 {
entry:
  %pgocount = load i64, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @__profc__Z1hv, i64 0, i64 0)
  %0 = add i64 %pgocount, 1
  store i64 %0, i64* getelementptr inbounds ([1 x i64], [1 x i64]* @__profc__Z1hv, i64 0, i64 0)
  ret void
}

; Function Attrs: noinline norecurse nounwind optnone uwtable
define dso_local i32 @main() #1 {
entry:
  %pgocount = load i64, i64* getelementptr inbounds ([2 x i64], [2 x i64]* @__profc_main, i64 0, i64 1)
  %0 = add i64 %pgocount, 1
  store i64 %0, i64* getelementptr inbounds ([2 x i64], [2 x i64]* @__profc_main, i64 0, i64 1)
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %pgocount12 = phi i64 [ %2, %for.inc ], [ 0, %entry ]
  %1 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %1, 100
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  call void @_Z1hv()
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %2 = add i64 %pgocount12, 1
  %3 = load i32, i32* %i, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %pgocount.promoted = load i64, i64* getelementptr inbounds ([2 x i64], [2 x i64]* @__profc_main, i64 0, i64 0)
  %4 = add i64 %pgocount.promoted, %pgocount12
  store i64 %4, i64* getelementptr inbounds ([2 x i64], [2 x i64]* @__profc_main, i64 0, i64 0)
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.instrprof.increment(i8*, i64, i32, i32) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline norecurse nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://github.com/llvm-mirror/clang.git 59f2009cd157fc96a0d558125405b98586cd83d2) (https://github.com/llvm-mirror/llvm.git 6a6735dc5864d58fa2735763609b42c59a520184)"}
