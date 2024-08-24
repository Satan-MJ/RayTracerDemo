//
// Implemented independently
//

#ifndef RAYTRACING_INTERVAL_HPP
#define RAYTRACING_INTERVAL_HPP

#include <algorithm>

class Interval {
public:
    // Empty interval
    Interval() {
        min = MAXFLOAT;
        max = -MAXFLOAT;
    }

    Interval(float _min, float _max) {
        min = _min;
        max = _max;
    }

    Interval(const Interval &i, const Interval &j) {
        min = std::min(i.getMin(), j.getMin());
        max = std::max(i.getMax(), j.getMax());
    }

    float getMin() const {
        return min;
    }

    float getMax() const {
        return max;
    }

    float getLength() const {
        return max - min;
    }

    Interval expand(float delta) const {
        return {min - delta, max + delta};
    }

    static Interval Empty() {
        return Interval();
    }

    static Interval Full() {
        return Interval(-1e4, 1e4);
    }

private:
    float min, max;
};

#endif //RAYTRACING_INTERVAL_HPP
