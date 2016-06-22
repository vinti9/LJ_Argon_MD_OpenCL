﻿/*! \file Ar_moleculardynamics.h
    \brief アルゴンに対して、分子動力学シミュレーションを行うクラスの宣言

    Copyright ©  2015 @dc1394 All Rights Reserved.
    This software is released under the BSD 2-Clause License.
*/

#ifndef _AR_MOLECULARDYNAMICS_H_
#define _AR_MOLECULARDYNAMICS_H_

#pragma once

#include "../vector4.h"
#include <array>                    // for std::array
#include <cstdint>                  // for std::int32_t
#include <vector>                   // for std::vector
#include "../myrandom/myrand.h"
#include <cmath>                    // for std::sqrt, std::pow
#include <boost/compute/container/vector.hpp>
#include <boost/compute/utility/source.hpp>
#include <boost/range/algorithm/generate.hpp>
#include <tbb/combinable.h>         // for tbb::combinable
#include <tbb/parallel_for.h>       // for tbb::parallel_for
#include <tbb/partitioner.h>        // for tbb::auto_partitioner
#include <iostream>

namespace moleculardynamics {
    //! A class.
    /*!
        アルゴンに対して、分子動力学シミュレーションを行うクラス
    */
    template <typename T>
    class Ar_moleculardynamics final {
        // #region コンストラクタ・デストラクタ

    public:
        //! A constructor.
        /*!
            コンストラクタ
        */
        Ar_moleculardynamics();

        //! A destructor.
        /*!
            デフォルトデストラクタ
        */
        ~Ar_moleculardynamics() = default;

        // #endregion コンストラクタ・デストラクタ

        // #region publicメンバ関数

        //! A public member function.
        /*!
            原子に働く力を計算する
        */
        void Calc_Forces();

        //! A public member function.
        /*!
            原子に働く力を計算する
        */
        void Calc_Forces_OpenCL();

        //! A public member function.
        /*!
            原子を移動させる
        */
        void Move_Atoms();
        
        //! A public member function.
        /*!
            原子を移動させる
        */
        void Move_Atoms_OpenCL();
        
        // #endregion publicメンバ関数

        // #region privateメンバ関数

    private:
        //! A private member function.
        /*!
            初期化する
        */
        void MD_init();
                
        //! A private member function.
        /*!
            原子の初期位置を決める
        */
        void MD_initPos();

        //! A private member function.
        /*!
            原子の初期速度を決める
        */
        void MD_initVel();

        //! A private member function (constant).
        /*!
            ノルムの二乗を求める
            \param x x座標
            \param y y座標
            \param z z座標
            \return ノルムの二乗
        */
        T norm2(T x, T y, T z) const
        {
            return x * x + y * y + z * z;
        }

        // #endregion privateメンバ関数

        // #region メンバ変数

    public:
        //! A private member variable (constant).
        /*!
            初期のスーパーセルの個数
        */
        static auto const FIRSTNC = 4;

        //! A private member variable (constant).
        /*!
            初期の格子定数のスケール
        */
        static T const FIRSTSCALE;

        //! A private member variable (constant).
        /*!
            初期温度（絶対温度）
        */
        static T const FIRSTTEMP;

    private:
        //! A private member variable (constant).
        /*!
            Woodcockの温度スケーリングの係数
        */
        static T const ALPHA;

        //! A private member variable (constant).
        /*!
            標準気圧
        */
        static T const ATM;

        //! A private member variable (constant).
        /*!
            アボガドロ定数
        */
        static T const AVOGADRO_CONSTANT;

        //! A private member variable (constant).
        /*!
            時間刻みΔt
        */
        static T const DT;
        
        //! A private member variable (constant).
        /*!
            1Hartree
        */
        static T const HARTREE;
        
        //! A private member variable (constant).
        /*!
            ボルツマン定数
        */
        static T const KB;
        
        //! A private member variable (constant).
        /*!
            アルゴン原子に対するσ
        */
        static T const SIGMA;

        //! A private member variable (constant).
        /*!
            アルゴン原子に対するτ
        */
        static T const TAU;

        //! A private member variable (constant).
        /*!
            アルゴン原子に対するε
        */
        static T const YPSILON;

        //! A private member variable (constant).
        /*!
            時間刻みの二乗
        */
        T const dt2;

        //! A private member variable.
        /*!
            OpenCLデバイス
        */
        boost::compute::device device_;

        //! A private member variable.
        /*!
            OpenCL context
        */
        boost::compute::context context_;

        //! A private member variable.
        /*!
            格子定数
        */
        T lat_;

        //! A private member variable (constant).
        /*!
            スーパーセルの個数
        */
        std::int32_t Nc_ = Ar_moleculardynamics::FIRSTNC;

        //! A private member variable.
        /*!
            n個目の原子に働く力
        */
        std::vector<utility::Vector4<T>> F_;
        
        //! A private member variable.
        /*!
            n個目の原子に働く力（デバイス側）
        */
        boost::compute::vector<boost::compute::float4_> F_dev_;

        //! A private member variable.
        /*!
            各原子に働く力を計算するカーネル
        */
        boost::compute::kernel kernel_force_;

        //! A private member variable.
        /*!
            各原子に働く力を初期化するカーネル
        */
        boost::compute::kernel kernel_initForce_;

        //! A private member variable.
        /*!
            修正Euler法で時間発展するカーネル
        */
        boost::compute::kernel kernel_move_atoms1_;

        //! A private member variable.
        /*!
            Verlet法で時間発展するカーネル
        */
        boost::compute::kernel kernel_move_atoms_;

        //! A private member variable.
        /*!
            MDのステップ数
        */
        std::int32_t MD_iter_;

        //! A private member variable (constant).
        /*!
            相互作用を計算するセルの個数
        */
        std::int32_t const ncp_ = 3;
        
        //! A private member variable.
        /*!
            原子数
        */
        std::int32_t NumAtom_;
        
        //! A private member variable.
        /*!
            周期境界条件の長さ
        */
        T periodiclen_;
        
        //! A private member variable.
        /*!
            OpenCLのキュー
        */
        boost::compute::command_queue queue_;

        //! A private member variable (constant).
        /*!
            カットオフ半径
        */
        T const rc_ = 2.5;

        //! A private member variable (constant).
        /*!
            カットオフ半径の2乗
        */
        T const rc2_;

        //! A private member variable (constant).
        /*!
            カットオフ半径の逆数の6乗
        */
        T const rcm6_;

        //! A private member variable (constant).
        /*!
            カットオフ半径の逆数の12乗
        */
        T const rcm12_;

        //! A private member variable.
        /*!
            格子定数のスケーリングの定数
        */
        T scale_ = Ar_moleculardynamics::FIRSTSCALE;
        
        //! A private member variable.
        /*!
            時間    
        */
        T t_;

        //! A private member variable.
        /*!
            計算された温度Tcalc
        */
        T Tc_;

        //! A private member variable.
        /*!
            与える温度Tgiven
        */
        T Tg_;
        
        //! A private member variable (constant).
        /*!
            運動エネルギー
        */
        T Uk_;

        //! A private member variable (constant).
        /*!
            ポテンシャルエネルギー
        */
        T Up_;

        //! A private member variable (constant).
        /*!
            全エネルギー
        */
        T Utot_;

        //! A private member variable (constant).
        /*!
            ポテンシャルエネルギーの打ち切り
        */
        T const Vrc_;
        
        //! A private member variable.
        /*!
            n個目の原子の速度
        */
        std::vector<utility::Vector4<T>> V_;
        
        //! A private member variable.
        /*!
            n個目の原子の速度（デバイス側）
        */
        boost::compute::vector<boost::compute::float4_> V_dev_;

        //! A private member variable.
        /*!
            n個目の原子の座標
        */
        std::vector<utility::Vector4<T>> r_;

        //! A private member variable.
        /*!
            n個目の原子の座標（デバイス側）
        */
        boost::compute::vector<boost::compute::float4_> r_dev_;

        //! A private member variable.
        /*!
            n個目の原子の初期座標
        */
        std::vector<utility::Vector4<T>> r1_;
        
        //! A private member variable.
        /*!
            n個目の原子の初期座標（デバイス側）
        */
        boost::compute::vector<boost::compute::float4_> r1_dev_;

        // #endregion メンバ変数

        // #region 禁止されたコンストラクタ・メンバ関数

        //! A private copy constructor (deleted).
        /*!
            コピーコンストラクタ（禁止）
        */
        Ar_moleculardynamics(Ar_moleculardynamics const &) = delete;

        //! A private member function (deleted).
        /*!
            operator=()の宣言（禁止）
            \param コピー元のオブジェクト（未使用）
            \return コピー元のオブジェクト
        */
        Ar_moleculardynamics & operator=(Ar_moleculardynamics const &) = delete;

        // #endregion 禁止されたコンストラクタ・メンバ関数
    };

    // #region static private 定数

    template <typename T>
    T const Ar_moleculardynamics<T>::FIRSTSCALE = 1.0;
    
    template <typename T>
    T const Ar_moleculardynamics<T>::FIRSTTEMP = 50.0;

    template <typename T>
    T const Ar_moleculardynamics<T>::ALPHA = 0.2;

    template <typename T>
    T const Ar_moleculardynamics<T>::ATM = 9.86923266716013E-6;

    template <typename T>
    T const Ar_moleculardynamics<T>::AVOGADRO_CONSTANT = 6.022140857E+23;

    template <typename T>
    T const Ar_moleculardynamics<T>::DT = 0.001;

    template <typename T>
    T const Ar_moleculardynamics<T>::HARTREE = 4.35974465054E-18;

    template <typename T>
    T const Ar_moleculardynamics<T>::KB = 1.3806488E-23;

    template <typename T>
    T const Ar_moleculardynamics<T>::SIGMA = 3.405E-10;

    template <typename T>
    T const Ar_moleculardynamics<T>::TAU =
        std::sqrt(0.039948 / Ar_moleculardynamics<T>::AVOGADRO_CONSTANT * Ar_moleculardynamics<T>::SIGMA * Ar_moleculardynamics<T>::SIGMA / Ar_moleculardynamics<T>::YPSILON);

    template <typename T>
    T const Ar_moleculardynamics<T>::YPSILON = 1.6540172624E-21;

    // #endregion static private 定数

    // #region コンストラクタ

    template <typename T>
    Ar_moleculardynamics<T>::Ar_moleculardynamics()
        :
        device_(boost::compute::system::default_device()),
        context_(device_),
        dt2(DT * DT),
        F_(Nc_ * Nc_ * Nc_ * 4),
        F_dev_(Nc_ * Nc_ * Nc_ * 4, context_),
        queue_(context_, device_),
        rc2_(rc_ * rc_),
        rcm6_(std::pow(rc_, -6.0)),
        rcm12_(std::pow(rc_, -12.0)),
        Tg_(Ar_moleculardynamics::FIRSTTEMP * Ar_moleculardynamics::KB / Ar_moleculardynamics::YPSILON),
        Vrc_(4.0 * (rcm12_ - rcm6_)),
        V_(Nc_ * Nc_ * Nc_ * 4),
        V_dev_(Nc_ * Nc_ * Nc_ * 4, context_),
        r_(Nc_ * Nc_ * Nc_ * 4),
        r_dev_(Nc_ * Nc_ * Nc_ * 4, context_),
        r1_(Nc_ * Nc_ * Nc_ * 4),
        r1_dev_(Nc_ * Nc_ * Nc_ * 4, context_)
    {
        // initalize parameters
        lat_ = std::pow(2.0, 2.0 / 3.0) * scale_;

        t_ = 0.0;
        MD_iter_ = 1;

        MD_initPos();
        MD_initVel();

        periodiclen_ = lat_ * static_cast<T>(Nc_);

        auto const init_force_source = BOOST_COMPUTE_STRINGIZE_SOURCE(kernel void init_force(__global float4 f[])
        {
            int const i = get_global_id(0);

            f[i] = (float4)(0.0f);
        });
                
        kernel_initForce_ = boost::compute::kernel::create_with_source(init_force_source, "init_force", context_);
        kernel_initForce_.set_args(F_dev_);

        auto const force_source = BOOST_COMPUTE_STRINGIZE_SOURCE(kernel void force(
            __global float4 f[],
            __global const float4 r[],
            int ncp,
            int numatom,
            float periodiclen,
            float rc2)
        {
            int const n = get_global_id(0);

            for (int m = 0; m < numatom; m++) {

                // ±ncp分のセル内の原子との相互作用を計算
                for (int i = -ncp; i <= ncp; i++) {
                    for (int j = -ncp; j <= ncp; j++) {
                        for (int k = -ncp; k <= ncp; k++) {
                            float4 s;
                            s.x = (float)(i) * periodiclen;
                            s.y = (float)(j) * periodiclen;
                            s.z = (float)(k) * periodiclen;
                            s.w = 0.0f;
                            
                            // 自分自身との相互作用を排除
                            if (n != m || i != 0 || j != 0 || k != 0) {
                                float4 d = r[n] - (r[m] + s);

                                float const r2 = d.x * d.x + d.y * d.y + d.z * d.z;
                                // 打ち切り距離内であれば計算
                                if (r2 <= rc2) {
                                    float const r = sqrt(r2);
                                    float const rm6 = 1.0 / (r2 * r2 * r2);
                                    float const rm7 = rm6 / r;
                                    float const rm12 = rm6 * rm6;
                                    float const rm13 = rm12 / r;

                                    float const Fr = 48.0 * rm13 - 24.0 * rm7;

                                    f[n] += d / r * (float4)(Fr);
                                }
                            }
                        }
                    }
                }
            }
        });

        kernel_force_ = boost::compute::kernel::create_with_source(force_source, "force", context_);
        kernel_force_.set_args(
            F_dev_,
            r_dev_,
            ncp_,
            NumAtom_,
            periodiclen_,
            rc2_);

        auto const move_atoms1_source = BOOST_COMPUTE_STRINGIZE_SOURCE(kernel void move_atoms1(
            __global float4 r[],
            __global float4 r1[],
            __global float4 V[],
            __global float4 const f[],
            float deltat,
            float s)
        {
            int const n = get_global_id(0);
            float4 const dt = (float4)(deltat);
            float4 const dt2 = dt * dt;
            
            r1[n] = r[n];

            // scaling of velocity
            V[n] *= (float4)(s);

            // update coordinates and velocity
            r[n] += dt * V[n] + (float4)(0.5) * f[n] * dt2;

            V[n] += (float4)(dt) * f[n];
        });

        kernel_move_atoms1_ = boost::compute::kernel::create_with_source(move_atoms1_source, "move_atoms1", context_);

        auto const move_atoms_source = BOOST_COMPUTE_STRINGIZE_SOURCE(kernel void move_atoms(
            __global float4 r[],
            __global float4 r1[],
            __global float4 V[],
            __global float4 const f[],
            float deltat)
        {
            int const n = get_global_id(0);
            float4 const dt = (float4)(deltat);
            float4 const dt2 = dt * dt;

            float4 const rtmp = r[n];

            // update coordinates and velocity
            r[n] = (float4)(2.0) * r[n] - r1[n] + f[n] * dt2;

            V[n] = (float4)(0.5) * (r[n] - r1[n]) / dt;

            r1[n] = rtmp;
        });

        kernel_move_atoms_ = boost::compute::kernel::create_with_source(move_atoms_source, "move_atoms", context_);
    }

    // #endregion コンストラクタ

    // #region publicメンバ関数

    template <typename T>
    void Ar_moleculardynamics<T>::Calc_Forces()
    {
        boost::compute::copy(
            F_.begin(), F_.end(),
            F_dev_.begin(),
            queue_
            );
        
        boost::compute::copy(
            r_.begin(), r_.end(),
            r_dev_.begin(),
            queue_
            );

        auto const event_init_force = queue_.enqueue_1d_range_kernel(
            kernel_initForce_,
            0,
            NumAtom_,
            16);
        event_init_force.wait();
        
        // 各原子に働く力の初期化
        //for (auto n = 0; n < NumAtom_; n++) {
        //    F_[n].data[0] = static_cast<T>(0);
        //    F_[n].data[1] = static_cast<T>(0);
        //    F_[n].data[2] = static_cast<T>(0);
        //}

        auto const event_force = queue_.enqueue_1d_range_kernel(
            kernel_force_,
            0,
            NumAtom_,
            16);
        event_force.wait();

        // ポテンシャルエネルギーの初期化
        Up_ = 0.0;
        tbb::combinable<T> Up;

        tbb::parallel_for(
            0,
            NumAtom_,
            1,
            [this, &Up](std::int32_t n) {
            for (auto m = 0; m < NumAtom_; m++) {

                // ±ncp_分のセル内の原子との相互作用を計算
                for (auto i = -ncp_; i <= ncp_; i++) {
                    for (auto j = -ncp_; j <= ncp_; j++) {
                        for (auto k = -ncp_; k <= ncp_; k++) {
                            auto const sx = static_cast<T>(i) * periodiclen_;
                            auto const sy = static_cast<T>(j) * periodiclen_;
                            auto const sz = static_cast<T>(k) * periodiclen_;

                            // 自分自身との相互作用を排除
                            if (n != m || i != 0 || j != 0 || k != 0) {
                                auto const dx = r_[n].data[0] - (r_[m].data[0] + sx);
                                auto const dy = r_[n].data[1] - (r_[m].data[1] + sy);
                                auto const dz = r_[n].data[2] - (r_[m].data[2] + sz);

                                auto const r2 = norm2(dx, dy, dz);
                                // 打ち切り距離内であれば計算
                                if (r2 <= rc2_) {
                                    auto const r = std::sqrt(r2);
                                    auto const rm6 = 1.0 / (r2 * r2 * r2);
                                    auto const rm7 = rm6 / r;
                                    auto const rm12 = rm6 * rm6;
                                    auto const rm13 = rm12 / r;

                                    auto const Fr = 48.0 * rm13 - 24.0 * rm7;

                                    //F_[n].data[0] += dx / r * Fr;
                                    //F_[n].data[1] += dy / r * Fr;
                                    //F_[n].data[2] += dz / r * Fr;

                                    // エネルギーの計算、ただし二重計算のために0.5をかけておく
                                    Up.local() += 0.5 * (4.0 * (rm12 - rm6) - Vrc_);
                                }
                            }
                        }
                    }
                }
            }
        },
            tbb::auto_partitioner());
        
        Up_ = Up.combine(std::plus<T>());

        boost::compute::copy(
            F_dev_.begin(), F_dev_.end(),
            F_.begin(),
            queue_
            );
    }

    template <typename T>
    void Ar_moleculardynamics<T>::Move_Atoms()
    {
        boost::compute::copy(
            F_.begin(), F_.end(),
            F_dev_.begin(),
            queue_
            );

        boost::compute::copy(
            r_.begin(), r_.end(),
            r_dev_.begin(),
            queue_
            );

        boost::compute::copy(
            r1_.begin(), r1_.end(),
            r1_dev_.begin(),
            queue_
            );

        boost::compute::copy(
            V_.begin(), V_.end(),
            V_dev_.begin(),
            queue_
            );

        // 運動エネルギーの初期化
        Uk_ = 0.0;

        // calculate temperture
        for (auto n = 0; n < NumAtom_; n++) {
            Uk_ += norm2(V_[n].data[0], V_[n].data[1], V_[n].data[2]);
        }

        // 運動エネルギーの計算
        Uk_ *= 0.5;

        // 全エネルギー（運動エネルギー+ポテンシャルエネルギー）の計算
        Utot_ = Uk_ + Up_;

        printf("全エネルギー = %.15f\n", Utot_);

        // 温度の計算
        Tc_ = Uk_ / (1.5 * static_cast<T>(NumAtom_));

        switch (MD_iter_) {
        case 1:
            {
                auto const s = std::sqrt((Tg_ + Ar_moleculardynamics::ALPHA * (Tc_ - Tg_)) / Tc_);
                // update the coordinates by the second order Euler method
                // 最初のステップだけ修正Euler法で時間発展
                //tbb::parallel_for(
                //    0,
                //    NumAtom_,
                //    1,
                //    [this, s](std::int32_t n) {
                //    r1_[n].data[0] = r_[n].data[0];
                //    r1_[n].data[1] = r_[n].data[1];
                //    r1_[n].data[2] = r_[n].data[2];

                //    // scaling of velocity
                //    V_[n].data[0] *= s;
                //    V_[n].data[1] *= s;
                //    V_[n].data[2] *= s;

                //    // update coordinates and velocity
                //    r_[n].data[0] += Ar_moleculardynamics::DT * V_[n].data[0] + 0.5 * F_[n].data[0] * dt2;
                //    r_[n].data[1] += Ar_moleculardynamics::DT * V_[n].data[1] + 0.5 * F_[n].data[1] * dt2;
                //    r_[n].data[2] += Ar_moleculardynamics::DT * V_[n].data[2] + 0.5 * F_[n].data[2] * dt2;

                //    V_[n].data[0] += Ar_moleculardynamics::DT * F_[n].data[0];
                //    V_[n].data[1] += Ar_moleculardynamics::DT * F_[n].data[1];
                //    V_[n].data[2] += Ar_moleculardynamics::DT * F_[n].data[2];
                //},
                //    tbb::auto_partitioner());
                
                kernel_move_atoms1_.set_args(
                    r_dev_,
                    r1_dev_,
                    V_dev_,
                    F_dev_,
                    Ar_moleculardynamics::DT,
                    s);

                auto const event_move_atoms1 = queue_.enqueue_1d_range_kernel(
                    kernel_move_atoms1_,
                    0,
                    NumAtom_,
                    16);
                event_move_atoms1.wait();
            }
            break;

        default:
            {
                // update the coordinates by the Verlet method
                //tbb::parallel_for(
                //    0,
                //    NumAtom_,
                //    1,
                //    [this](std::int32_t n) {
                //    auto const rtmp = r_[n].data;

                //    // update coordinates and velocity
                //    r_[n].data[0] = 2.0 * r_[n].data[0] - r1_[n].data[0] + F_[n].data[0] * dt2;
                //    r_[n].data[1] = 2.0 * r_[n].data[1] - r1_[n].data[1] + F_[n].data[1] * dt2;
                //    r_[n].data[2] = 2.0 * r_[n].data[2] - r1_[n].data[2] + F_[n].data[2] * dt2;

                //    V_[n].data[0] = 0.5 * (r_[n].data[0] - r1_[n].data[0]) / Ar_moleculardynamics::DT;
                //    V_[n].data[1] = 0.5 * (r_[n].data[1] - r1_[n].data[1]) / Ar_moleculardynamics::DT;
                //    V_[n].data[2] = 0.5 * (r_[n].data[2] - r1_[n].data[2]) / Ar_moleculardynamics::DT;

                //    r1_[n].data[0] = rtmp[0];
                //    r1_[n].data[1] = rtmp[1];
                //    r1_[n].data[2] = rtmp[2];
                //},
                //    tbb::auto_partitioner());
                kernel_move_atoms_.set_args(
                    r_dev_,
                    r1_dev_,
                    V_dev_,
                    F_dev_,
                    Ar_moleculardynamics::DT);

                auto const event_move_atoms = queue_.enqueue_1d_range_kernel(
                    kernel_move_atoms_,
                    0,
                    NumAtom_,
                    16);
                event_move_atoms.wait();
            }
            break;
        }

        boost::compute::copy(
            r_dev_.begin(), r_dev_.end(),
            r_.begin(),
            queue_
            );

        boost::compute::copy(
            r1_dev_.begin(), r1_dev_.end(),
            r1_.begin(),
            queue_
            );

        boost::compute::copy(
            V_dev_.begin(), V_dev_.end(),
            V_.begin(),
            queue_
            );

        // consider the periodic boundary condination
        // セルの外側に出たら座標をセル内に戻す
        tbb::parallel_for(
            0,
            NumAtom_,
            1,
            [this](std::int32_t n) {
            for (auto i = 0; i < 3; i++) {
                if (r_[n].data[i] > periodiclen_) {
                    r_[n].data[i] -= periodiclen_;
                    r1_[n].data[i] -= periodiclen_;
                }
                else if (r_[n].data[i] < 0.0) {
                    r_[n].data[i] += periodiclen_;
                    r1_[n].data[i] += periodiclen_;
                }
            }
        },
            tbb::auto_partitioner());

        // 繰り返し回数と時間を増加
        t_ = static_cast<T>(MD_iter_) * Ar_moleculardynamics::DT;
        MD_iter_++;
    }

    // #endregion publicメンバ関数

    // #region privateメンバ関数

    template <typename T>
    void Ar_moleculardynamics<T>::MD_initPos()
    {
        T sx, sy, sz;
        auto n = 0;

        for (auto i = 0; i < Nc_; i++) {
            for (auto j = 0; j < Nc_; j++) {
                for (auto k = 0; k < Nc_; k++) {
                    // 基本セルをコピーする
                    sx = static_cast<T>(i) * lat_;
                    sy = static_cast<T>(j) * lat_;
                    sz = static_cast<T>(k) * lat_;

                    // 基本セル内には4つの原子がある
                    r_[n].data[0] = sx;
                    r_[n].data[1] = sy;
                    r_[n].data[2] = sz;
                    n++;

                    r_[n].data[0] = 0.5 * lat_ + sx;
                    r_[n].data[1] = 0.5 * lat_ + sy;
                    r_[n].data[2] = sz;
                    n++;

                    r_[n].data[0] = sx;
                    r_[n].data[1] = 0.5 * lat_ + sy;
                    r_[n].data[2] = 0.5 * lat_ + sz;
                    n++;

                    r_[n].data[0] = 0.5 * lat_ + sx;
                    r_[n].data[1] = sy;
                    r_[n].data[2] = 0.5 * lat_ + sz;
                    n++;
                }
            }
        }

        NumAtom_ = n;

        // move the center of mass to the origin
        // 系の重心を座標系の原点とする
        sx = 0.0;
        sy = 0.0;
        sz = 0.0;

        for (auto n = 0; n < NumAtom_; n++) {
            sx += r_[n].data[0];
            sy += r_[n].data[1];
            sz += r_[n].data[2];
        }

        sx /= static_cast<T>(NumAtom_);
        sy /= static_cast<T>(NumAtom_);
        sz /= static_cast<T>(NumAtom_);

        for (auto n = 0; n < NumAtom_; n++) {
            r_[n].data[0] -= sx;
            r_[n].data[1] -= sy;
            r_[n].data[2] -= sz;
        }
    }

    template <typename T>
    void Ar_moleculardynamics<T>::MD_initVel()
    {
        auto const v = std::sqrt(3.0 * Tg_);

        myrandom::MyRand mr(-1.0, 1.0);

        auto const generator4 = [this, &mr, v]() {
            T rndX = mr.myrand();
            T rndY = mr.myrand();
            T rndZ = mr.myrand();
            T const tmp = 1.0 / std::sqrt(norm2(rndX, rndY, rndZ));
            rndX *= tmp;
            rndY *= tmp;
            rndZ *= tmp;
            
            // 方向はランダムに与える
            return utility::Vector4<T>(v * rndX, v * rndY, v * rndZ);
        };

        boost::generate(V_, generator4);

        auto sx = 0.0;
        auto sy = 0.0;
        auto sz = 0.0;

        for (auto n = 0; n < NumAtom_; n++) {
            sx += V_[n].data[0];
            sy += V_[n].data[1];
            sz += V_[n].data[2];
        }

        sx /= static_cast<T>(NumAtom_);
        sy /= static_cast<T>(NumAtom_);
        sz /= static_cast<T>(NumAtom_);

        // 重心の並進運動を避けるために、速度の和がゼロになるように補正
        for (auto n = 0; n < NumAtom_; n++) {
            V_[n].data[0] -= sx;
            V_[n].data[1] -= sy;
            V_[n].data[2] -= sz;
        }
    }

    // #endregion privateメンバ関数
}

#endif      // _AR_MOLECULARDYNAMICS_H_