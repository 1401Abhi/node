// Copyright 2023 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/turboshaft/wasm-dead-code-elimination-phase.h"

#include "src/compiler/js-heap-broker.h"
#include "src/compiler/turboshaft/copying-phase.h"
#include "src/compiler/turboshaft/dead-code-elimination-reducer.h"
#include "src/compiler/turboshaft/duplication-optimization-reducer.h"
#include "src/compiler/turboshaft/load-store-simplification-reducer.h"
#include "src/compiler/turboshaft/phase.h"
#include "src/compiler/turboshaft/stack-check-reducer.h"
#include "src/compiler/turboshaft/value-numbering-reducer.h"

namespace v8::internal::compiler::turboshaft {

void WasmDeadCodeEliminationPhase::Run(Zone* temp_zone) {
  UnparkedScopeIfNeeded scope(PipelineData::Get().broker(), DEBUG_BOOL);

  // The value numbering ensures that load with similar patterns in the complex
  // loads can share those calculations.
  CopyingPhase<DeadCodeEliminationReducer, StackCheckReducer,
               LoadStoreSimplificationReducer,
               // We make sure that DuplicationOptimizationReducer runs after
               // LoadStoreSimplificationReducer, so that it can optimize
               // Loads/Stores produced by LoadStoreSimplificationReducer
               // (which, for simplificy, doesn't use the Assembler helper
               // methods, but only calls Next::ReduceLoad/Store).
               DuplicationOptimizationReducer,
               VariableReducerHotfix,
               ValueNumberingReducer>::Run<false>(temp_zone);
}

}  // namespace v8::internal::compiler::turboshaft
