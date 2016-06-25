#include "checkpoint.h"
#include "moleculardynamics/Ar_moleculardynamics.h"

namespace {
    static auto constexpr LOOP = 25;
}

int main()
{
    checkpoint::CheckPoint cp;
    cp.checkpoint("処理開始", __LINE__);

    moleculardynamics::Ar_moleculardynamics<float> armd;
    
    cp.checkpoint("初期化処理", __LINE__);

    for (auto i = 0; i < LOOP; i++) {
        armd.Calc_Forces();
        armd.Move_Atoms();
    }

    cp.checkpoint("TBBで並列化", __LINE__);

    armd.reset();

    cp.checkpoint("再初期化", __LINE__);

    for (auto i = 0; i < LOOP; i++) {
        armd.Calc_Forces_OpenCL();
        armd.Move_Atoms_OpenCL();
    }

    cp.checkpoint("OpenCLで並列化", __LINE__);

    cp.checkpoint_print();

    return 0;
}