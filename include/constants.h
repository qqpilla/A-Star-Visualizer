#pragma once

enum 
{
    W_Side = 800,
    G_Resolution_Side = 32
};

inline float Normalized(float value)
{
    return (value / float(W_Side)) * 2.0f - 1.0f;
}