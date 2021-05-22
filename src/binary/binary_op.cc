/* Copyright 2021 NVIDIA Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "binary/binary_op.h"
#include "binary/binary_op_template.inl"

namespace legate {
namespace numpy {

using namespace Legion;

template <BinaryOpCode OP_CODE, LegateTypeCode CODE, int DIM>
struct BinaryOpImplBody<VariantKind::CPU, OP_CODE, CODE, DIM> {
  using OP  = BinaryOp<OP_CODE, CODE>;
  using ARG = legate_type_of<CODE>;
  using RES = std::result_of_t<OP(ARG, ARG)>;

  void operator()(OP func,
                  AccessorWO<RES, DIM> out,
                  AccessorRO<ARG, DIM> in1,
                  AccessorRO<ARG, DIM> in2,
                  const Pitches<DIM - 1> &pitches,
                  const Rect<DIM> &rect,
                  bool dense) const
  {
    if (dense) {
      size_t volume = rect.volume();
      auto outptr   = out.ptr(rect);
      auto in1ptr   = in1.ptr(rect);
      auto in2ptr   = in2.ptr(rect);
      for (size_t idx = 0; idx < volume; ++idx) outptr[idx] = func(in1ptr[idx], in2ptr[idx]);
    } else {
      CPULoop<DIM>::binary_loop(func, out, in1, in2, rect);
    }
  }
};

void deserialize(Deserializer &ctx, BinaryOpArgs &args)
{
  deserialize(ctx, args.op_code);
  deserialize(ctx, args.shape);
  deserialize(ctx, args.out);
  deserialize(ctx, args.in1);
  deserialize(ctx, args.in2);
  deserialize(ctx, args.args);
}

/*static*/ void BinaryOpTask::cpu_variant(const Task *task,
                                          const std::vector<PhysicalRegion> &regions,
                                          Context context,
                                          Runtime *runtime)
{
  binary_op_template<VariantKind::CPU>(task, regions, context, runtime);
}

namespace  // unnamed
{
static void __attribute__((constructor)) register_tasks(void) { BinaryOpTask::register_variants(); }
}  // namespace

}  // namespace numpy
}  // namespace legate
