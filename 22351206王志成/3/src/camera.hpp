#pragma once

#include <array>
#include <chrono>

#include <nvmath/nvmath.h>

#include "boundingBox.hpp"

namespace std
{
    inline float sign(float s)
    {
        return (s < 0.f) ? -1.f : 1.f;
    }
};

class Camera
{
public:
    enum eCameraProjectionType
    {
        CAMERA_PROJECTION_TYPE_ORTHO,
        CAMERA_PROJECTION_TYPE_PERSPECTIVE
    };

    enum eCameraActionMode
    {
        CAMERA_ACTION_MODE_FLY,
        CAMERA_ACTION_MODE_FOCUS_ON_OBJECT
    };

    enum eCameraActions
    {
        CAMERA_ACTION_NONE,
        CAMERA_ACTION_ORBIT,
        CAMERA_ACTION_LOOK_AROUND,
        CAMERA_ACTION_DOLLY,
        CAMERA_ACTION_PAN
    };

    auto getLookat() const { return std::array{m_eye, m_target, m_up}; }
    const nvmath::vec2f &getClipPlanes() const { return m_clipPlanes; }
    const eCameraProjectionType &getProjectionType() const { return m_type; }
    const float &getFov() const { return m_fov; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    const nvmath::vec2f &getMousePosition() const { return m_mousePosition; }
    nvmath::mat4f getViewMatrix() const { return m_viewMatrix; }
    nvmath::mat4f getProjectionMatrix() const { return nvmath::perspectiveVK(m_fov, (float)m_height / m_width, m_clipPlanes.x, m_clipPlanes.y); }
    const eCameraActionMode &getActionMode() const { return m_mode; }

    void setClipPlanes(const nvmath::vec2f &clip) { m_clipPlanes = clip; }
    void setProjectionType(const eCameraProjectionType &type) { m_type = type; }
    void setFov(const float &fov) { m_fov = std::min(std::max(fov, 0.01f), 179.0f); }
    void setWindowSize(int w, int h) { m_width = w, m_height = h; }
    void setMousePosition(const nvmath::vec2f &pos) { m_mousePosition = pos; }
    void setActionMode(const eCameraActionMode &mode) { m_mode = mode; }

protected:
    void update() { m_viewMatrix = nvmath::look_at(m_eye, m_target, m_up); }

    // Do panning: movement parallels to the screen
    void pan(float dx, float dy)
    {
        if (m_mode == CAMERA_ACTION_MODE_FLY)
        {
            dx *= -1, dy *= -1;
        }

        nvmath::vec3f z(m_eye - m_target);
        float length = static_cast<float>(nvmath::length(z)) / 0.785f; // 45 degrees
        z = nvmath::normalize(z);
        nvmath::vec3f x = nvmath::cross(m_up, z);
        x = nvmath::normalize(x);
        nvmath::vec3f y = nvmath::cross(z, x);
        y = nvmath::normalize(y);
        x *= -dx * length;
        y *= dy * length;

        m_eye += x + y;
        m_target += x + y;
    }
    // Do orbiting: rotation around the center of interest. If invert, the interest orbit around the camera position
    void orbit(float dx, float dy, bool invert = false)
    {
        if (dx == 0 && dy == 0)
            return;

        // Full width will do a full turn
        dx *= nv_two_pi;
        dy *= nv_two_pi;

        // Get the camera
        nvmath::vec3f origin(invert ? m_eye : m_target);
        nvmath::vec3f position(invert ? m_target : m_eye);

        // Get the length of sight
        nvmath::vec3f centerToEye(position - origin);
        float radius = nvmath::length(centerToEye);
        centerToEye = nvmath::normalize(centerToEye);

        nvmath::mat4f rot_x, rot_y;

        // Find the rotation around the UP axis (Y)
        nvmath::vec3f axe_z(nvmath::normalize(centerToEye));
        rot_y = nvmath::mat4f().as_rot(-dx, m_up);

        // Apply the (Y) rotation to the eye-center vector
        nvmath::vec4f vect_tmp = rot_y * nvmath::vec4f(centerToEye.x, centerToEye.y, centerToEye.z, 0);
        centerToEye = nvmath::vec3f(vect_tmp.x, vect_tmp.y, vect_tmp.z);

        // Find the rotation around the X vector: cross between eye-center and up (X)
        nvmath::vec3f axe_x = nvmath::cross(m_up, axe_z);
        axe_x = nvmath::normalize(axe_x);
        rot_x = nvmath::mat4f().as_rot(-dy, axe_x);

        // Apply the (X) rotation to the eye-center vector
        vect_tmp = rot_x * nvmath::vec4f(centerToEye.x, centerToEye.y, centerToEye.z, 0);
        nvmath::vec3f vect_rot(vect_tmp.x, vect_tmp.y, vect_tmp.z);
        if (std::sign(vect_rot.x) == std::sign(centerToEye.x))
            centerToEye = vect_rot;

        // Make the vector as long as it was originally
        centerToEye *= radius;

        // Finding the new position
        nvmath::vec3f newPosition = centerToEye + origin;

        if (!invert)
            m_eye = newPosition; // Normal: change the position of the camera
        else
            m_target = newPosition; // Inverted: change the interest point
    }
    // Do dolly: movement toward the interest.
    void dolly(float dx, float dy)
    {
        nvmath::vec3f z = m_target - m_eye;
        float length = static_cast<float>(nvmath::length(z));

        // We are at the point of interest, and don't know any direction, so do nothing!
        if (length < 0.000001f)
            return;

        // Use the larger movement.
        float dd;
        if (m_mode == CAMERA_ACTION_MODE_FLY)
            dd = -dy;
        else
            dd = fabs(dx) > fabs(dy) ? dx : -dy;
        float factor = m_speed * dd;

        // Adjust speed based on distance.
        if (m_mode == CAMERA_ACTION_MODE_FOCUS_ON_OBJECT)
        {
            // Don't move over the point of interest.
            if (factor >= 1.0f)
                return;

            z *= factor;
        }
        else
            // Normalize the Z vector and make it faster
            z *= factor / length * 10.0f;

        m_eye += z;

        // In fly mode, the interest moves with us.
        if (m_mode == CAMERA_ACTION_MODE_FLY)
            m_target += z;
    }

    double getSystemTime()
    {
        auto now(std::chrono::system_clock::now());
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0;
    }

    nvmath::vec3f computeBezier(float t, nvmath::vec3f &p0, nvmath::vec3f &p1, nvmath::vec3f &p2)
    {
        float u = 1.f - t;
        float tt = t * t;
        float uu = u * u;

        nvmath::vec3f p = uu * p0; // first term
        p += 2 * u * t * p1;       // second term
        p += tt * p2;              // third term

        return p;
    }
    void findBezierPoints()
    {
        nvmath::vec3f p0 = m_eye;
        nvmath::vec3f p2 = m_eyeTo;
        nvmath::vec3f p1, pc;

        // point of interest
        nvmath::vec3f pi = (m_targetTo + m_target) * 0.5f;

        nvmath::vec3f p02 = (p0 + p2) * 0.5f;                      // mid p0-p2
        float radius = (length(p0 - pi) + length(p2 - pi)) * 0.5f; // Radius for p1
        nvmath::vec3f p02pi(p02 - pi);                             // Vector from interest to mid point
        p02pi.normalize();
        p02pi *= radius;
        pc = pi + p02pi;                       // Calculated point to go through
        p1 = 2.f * pc - p0 * 0.5f - p2 * 0.5f; // Computing p1 for t=0.5
        p1.y = p02.y;                          // Clamping the P1 to be in the same height as p0-p2

        m_bezier[0] = p0;
        m_bezier[1] = p1;
        m_bezier[2] = p2;
    }

public:
    void updateAnim()
    {
        auto elapse = static_cast<float>(getSystemTime() - m_start_time) / 1000.f;

        // Key animation
        if (m_key_vec != nvmath::vec3f(0, 0, 0))
        {
            m_eye += m_key_vec * elapse * 1000.f;
            m_target += m_key_vec * elapse * 1000.f;
            update();
            m_start_time = getSystemTime();
            return;
        }

        // Camera moving to new position
        if (m_anim_done)
            return;

        float t = std::min(elapse / float(m_duration), 1.0f);
        // Evaluate polynomial (smoother step from Perlin)
        t = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        if (t >= 1.0f)
        {
            m_eye = m_eyeTo;
            m_target = m_targetTo;
            m_up = m_upTo;
            m_fov = m_fovTo;
            m_anim_done = true;
            return;
        }

        // Interpolate camera position and interest
        // The distance of the camera between the interest is preserved to
        // create a nicer interpolation
        nvmath::vec3f vpos, vint, vup;
        m_target = nvmath::lerp(t, m_targetFrom, m_targetTo);
        m_up = nvmath::lerp(t, m_upFrom, m_upTo);
        m_eye = computeBezier(t, m_bezier[0], m_bezier[1], m_bezier[2]);
        m_fov = nvmath::lerp(t, m_fovFrom, m_fovTo);

        update();
    }

    void setLookat(const nvmath::vec3f &eye, const nvmath::vec3f &center, const nvmath::vec3f &up, bool instantSet = true)
    {
        m_anim_done = true;

        if (instantSet)
        {
            m_eye = eye;
            m_target = center;
            m_up = up;
            update();
        }
        else if (m_eye != eye || m_target != center || m_up != up)
        {
            m_eyeFrom = m_eye;
            m_targetFrom = m_target;
            m_upFrom = m_up;
            m_fovFrom = m_fov;
            m_eyeTo = eye;
            m_targetTo = center;
            m_upTo = up;
            m_fovTo = m_fov;
            m_anim_done = false;
            m_start_time = getSystemTime();
            findBezierPoints();
        }
    }

    void fit(const BoundingBox &box, const nvmath::mat4f &modelMatrix = nvmath::mat4f(1), bool instantFit = true, bool tight = false, float aspect = 1.0f)
    {
        nvmath::vec4f temp = modelMatrix * nvmath::vec4f{box.minPoint, 1.f};
        const nvmath::vec3f minPoint = {temp.x, temp.y, temp.z};
        temp = modelMatrix * nvmath::vec4f{box.maxPoint, 1.f};
        const nvmath::vec3f maxPoint = {temp.x, temp.y, temp.z};
        const nvmath::vec3f boxHalfSize = (maxPoint - minPoint) * .5f;
        const nvmath::vec3f boxCenter = minPoint + boxHalfSize;

        float offset = 0;
        float yfov = m_fov;
        float xfov = m_fov * aspect;

        if (!tight)
        {
            // Using the bounding sphere
            float radius = nvmath::length(boxHalfSize);
            if (aspect > 1.f)
                offset = radius / sin(nv_to_rad * yfov * 0.5f);
            else
                offset = radius / sin(nv_to_rad * xfov * 0.5f);
        }
        else
        {
            nvmath::mat4f mView = nvmath::look_at(m_eye, boxCenter, m_up);
            mView.set_translate({0, 0, 0}); // Keep only rotation

            for (int i = 0; i < 8; i++)
            {
                nvmath::vec3f vct(i & 1 ? boxHalfSize.x : -boxHalfSize.x, i & 2 ? boxHalfSize.y : -boxHalfSize.y,
                                  i & 4 ? boxHalfSize.z : -boxHalfSize.z);
                vct = nvmath::vec3f(mView * vct);

                if (vct.z < 0) // Take only points in front of the center
                {
                    // Keep the largest offset to see that vertex
                    offset = std::max(fabs(vct.y) / tan(nv_to_rad * yfov * 0.5f) + fabs(vct.z), offset);
                    offset = std::max(fabs(vct.x) / tan(nv_to_rad * xfov * 0.5f) + fabs(vct.z), offset);
                }
            }
        }

        // Re-position the camera
        auto viewDir = nvmath::normalize(m_eye - m_target);
        auto veye = boxCenter + viewDir * offset;
        setLookat(veye, boxCenter, m_up, instantFit);
    }

    void motion(int x, int y, eCameraActions action = eCameraActions::CAMERA_ACTION_NONE)
    {
        float dx = float(x - m_mousePosition[0]) / float(m_width);
        float dy = float(y - m_mousePosition[1]) / float(m_height);

        switch (action)
        {
        case eCameraActions::CAMERA_ACTION_ORBIT:
            orbit(dx, dy, false);
            break;
        case eCameraActions::CAMERA_ACTION_LOOK_AROUND:
            orbit(dx, -dy, true);
            break;
        case eCameraActions::CAMERA_ACTION_DOLLY:
            dolly(dx, dy);
            break;
        case eCameraActions::CAMERA_ACTION_PAN:
            pan(dx, dy);
            break;
        }

        // Resetting animation
        m_anim_done = true;

        update();

        m_mousePosition[0] = static_cast<float>(x);
        m_mousePosition[1] = static_cast<float>(y);
    }
    void keyMotion(float dx, float dy, eCameraActions action)
    {
        if (action == eCameraActions::CAMERA_ACTION_NONE)
        {
            m_key_vec = {0, 0, 0};
            return;
        }

        auto d = nvmath::normalize(m_target - m_eye);
        dx *= m_speed * 2.f;
        dy *= m_speed * 2.f;

        nvmath::vec3f key_vec;
        if (action == eCameraActions::CAMERA_ACTION_DOLLY)
            key_vec = d * dx;
        else if (action == eCameraActions::CAMERA_ACTION_PAN)
        {
            auto r = nvmath::cross(d, m_up);
            key_vec = r * dx + m_up * dy;
        }

        m_key_vec += key_vec;

        // Resetting animation
        m_start_time = getSystemTime();
    }

    eCameraActions mouseMove(int x, int y, bool lmb, bool mmb, bool rmb)
    {
        eCameraActions curAction = eCameraActions::CAMERA_ACTION_NONE;
        if (lmb)
        {
            curAction = m_mode == eCameraActionMode::CAMERA_ACTION_MODE_FOCUS_ON_OBJECT ? eCameraActions::CAMERA_ACTION_ORBIT : eCameraActions::CAMERA_ACTION_LOOK_AROUND;
        }
        else if (mmb)
            curAction = eCameraActions::CAMERA_ACTION_PAN;
        else if (rmb)
            curAction = eCameraActions::CAMERA_ACTION_DOLLY;

        if (curAction != eCameraActions::CAMERA_ACTION_NONE)
            motion(x, y, curAction);

        return curAction;
    }
    void wheel(int value)
    {
        float fval(static_cast<float>(value));
        float dx = (fval * fabsf(fval)) / static_cast<float>(m_width);

        dolly(dx * m_speed, dx * m_speed);
        update();
    }

protected:
    // basic
    nvmath::vec3f m_eye{10, 10, 10};
    nvmath::vec3f m_target{0, 0, 0};
    nvmath::vec3f m_up{0, 1, 0};
    nvmath::vec2f m_clipPlanes{0.001f, 100000000.f};
    eCameraProjectionType m_type{CAMERA_PROJECTION_TYPE_PERSPECTIVE};
    float m_fov{60.0f};

    nvmath::mat4f m_viewMatrix = nvmath::mat4f(1);

    // Animation
    nvmath::vec3f m_eyeFrom{};
    nvmath::vec3f m_targetFrom{};
    nvmath::vec3f m_upFrom{};
    float m_fovFrom{};
    nvmath::vec3f m_eyeTo{};
    nvmath::vec3f m_targetTo{};
    nvmath::vec3f m_upTo{};
    float m_fovTo{};
    std::array<nvmath::vec3f, 3> m_bezier;
    double m_start_time = 0;
    double m_duration = 0.5;
    bool m_anim_done{true};
    nvmath::vec3f m_key_vec{0, 0, 0};

    // Screen
    int m_width = 1;
    int m_height = 1;

    // Other
    float m_speed = 3.f;
    nvmath::vec2f m_mousePosition{0.f, 0.f};

    bool m_button = false; // Button pressed
    bool m_moving = false; // Mouse is moving
    float m_tbsize = 0.8f; // Trackball size;

    eCameraActionMode m_mode{CAMERA_ACTION_MODE_FLY};
};