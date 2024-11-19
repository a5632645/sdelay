#pragma once
#include <cmath>
#include <ranges>
#include <xsimd/xsimd.hpp>

#define ALIGNED32  __declspec(align(32))

/*
* 堆叠N个全通的级联滤波器
*/
class StackAllPassFilter {
public:
    static constexpr auto kNumStack = 8;

    StackAllPassFilter() = default;
    StackAllPassFilter(float theta[kNumStack], float radius[kNumStack], float bw[kNumStack]) {
        Set(theta, radius, bw);
    }

    void Process(float* input, int num_samples) {
        using batch = xsimd::batch<float, xsimd::avx2>;

        auto x2 = batch::load_aligned(&x2_[0]);
        auto x1 = batch::load_aligned(&x1_[0]);
        auto y2 = batch::load_aligned(&y2_[0]);
        auto y1 = batch::load_aligned(&y1_[0]);
        auto ca = batch::load_aligned(&a_[0]);
        auto cb = batch::load_aligned(&b_[0]);
        ALIGNED32 float tmp[kNumStack]{};
        ALIGNED32 float x_tmp[kNumStack]{};
        ALIGNED32 float y_tmp[kNumStack]{};

        for (int n = 0; n < num_samples; ++n) {
            auto tv = x2 + x1 * ca - y1 * ca - y2 * cb;
            tv.store_aligned(&tmp[0]);
            float t2 = input[n];
            for (int i = 0; i < kNumStack; ++i) {
                x_tmp[i] = t2;
                auto filter_i_output = tmp[i] + t2 * b_[i];
                y_tmp[i] = filter_i_output;
                t2 = filter_i_output;
            }
            // output
            input[n] = t2;

            // write and swap register
            y2 = y1;
            y1 = batch::load_aligned(&y_tmp[0]);
            x2 = x1;
            x1 = batch::load_aligned(&x_tmp[0]);
        }

        // store
        y2.store_aligned(&y2_[0]);
        y1.store_aligned(&y1_[0]);
        x2.store_aligned(&x2_[0]);
        x1.store_aligned(&x1_[0]);
    }

    void Set(float theta[kNumStack], float radius[kNumStack], float bw[kNumStack]) {
        // calc coeff
        for (int i = 0; i < kNumStack; ++i) {
            b_[i] = radius[i] * radius[i];
            a_[i] = -2 * radius[i] * std::cos(theta[i]);
        }

        // copy additional info
        std::ranges::copy(theta, theta + kNumStack, theta_);
        std::ranges::copy(radius, radius + kNumStack, radius_);
        std::ranges::copy(bw, bw + kNumStack, bw_);
    }

    float GetBw(size_t i) const {
        return bw_[i];
    }

    float GetTheta(size_t i) const {
        return theta_[i];
    }

    float GetGroupDelay(float w) const {
        constexpr auto interval = 1.0f / 10000.0f;
        return -(GetPhaseResponse(w + interval) - GetPhaseResponse(w)) / interval;
    }

    float GetPhaseResponse(float w) const {
        float ret = 0.0f;
        for (int i = 0; i < kNumStack; ++i) {
            ret += GetSinglePhaseResponse(w, theta_[i], radius_[i]);
        }
        return ret;
    }

    void PaincFb() {
        std::fill(x2_, x2_ + kNumStack, 0.0f);
        std::fill(x1_, x1_ + kNumStack, 0.0f);
        std::fill(y2_, y2_ + kNumStack, 0.0f);
        std::fill(y1_, y1_ + kNumStack, 0.0f);
    }
private:
    inline static float GetSinglePhaseResponse(float w, float theta, float radius) {
        return -2 * w
            - 2 * std::atan(radius * std::sin(w - theta) / (1 - radius * std::cos(w - theta)))
            - std::atan(radius * std::sin(w + theta) / (1 - radius * std::cos(w + theta)));
    }

    float theta_[kNumStack]{};
    float radius_[kNumStack]{};
    float bw_[kNumStack]{};

    // coeff
    ALIGNED32 float a_[kNumStack]{};
    ALIGNED32 float b_[kNumStack]{};

    // data
    ALIGNED32 float x2_[kNumStack]{};
    ALIGNED32 float x1_[kNumStack]{};
    ALIGNED32 float y2_[kNumStack]{};
    ALIGNED32 float y1_[kNumStack]{};
};
