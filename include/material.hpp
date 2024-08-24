#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include "texture.hpp"
#include "random.hpp"
#include "transformation.hpp"
#include "object_pdf.hpp"
#include "hemisphere_pdf.hpp"
#include "mixture_pdf.hpp"
#include "cosine_pdf.hpp"

class Object3D;

const float rayEpsilon = 0.0001f;

static Vector3f reflect(const Vector3f &v, const Vector3f &n) {
    return v - 2 * Vector3f::dot(v, n) * n;
}

static bool refract(const Vector3f &v, const Vector3f &n, float niOverNt, Vector3f &refracted) {
    Vector3f uv = v.normalized();
    float dt = Vector3f::dot(uv, n);
    float discriminant = 1.0f - niOverNt * niOverNt * (1.0f - dt * dt);
    if (discriminant > 0) {
        refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

static float schlick(float cosine, float refractiveIndex) {
    float r0 = (1 - refractiveIndex) / (1 + refractiveIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

//
// Referencing https://raytracing.github.io
//
class Material {
public:
    explicit Material(ImageTexture* normalMap) : normalMap(normalMap) {}

    virtual ~Material() = default;

    // return the emmisive color of the material
    virtual Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const {
        attenuation = Vector3f::ZERO;
        scattered = Ray(Vector3f::ZERO, Vector3f::ZERO);

        return Vector3f::ZERO;
    }

    virtual bool isEmissive() const {
        return false;
    }

    ImageTexture* getNormalMap() const {
        return normalMap;
    }

protected:
    ImageTexture* normalMap;

    Vector3f getNormal(const Vector3f &objectNormal, float u, float v) const {
        if (normalMap == nullptr) {
            return objectNormal;
        }

        Vector3f normal = normalMap->getColor(u, v) - Vector3f(0.5f, 0.5f, 0);
        Matrix3f TBNRotation = orthonormalBasis(objectNormal);
        return (TBNRotation * normal).normalized();
    }
};

//
// Referencing https://raytracing.github.io
//
class IsotropicMaterial : public Material {
public:
    explicit IsotropicMaterial(Texture* texture) : texture(texture), Material(nullptr) {}

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        Vector3f dir = randomUnitVector3d();
        scattered = Ray(ray.pointAtParameter(hit.getT()), dir);

        float pdf = 1 / (4 * M_PI);
        attenuation = texture->getColor(hit.getU(), hit.getV()) * getScatteringPdf(ray, hit, dir) / pdf;

        return Vector3f::ZERO;
    }

private:
    float getScatteringPdf(const Ray &ray, const Hit &hit, const Vector3f &scatteredDir) const {
        return 1 / (4 * M_PI);
    }

    Texture* texture;
};

//
// Implemented independently
//
class EmissiveMaterial : public Material {
public:
    explicit EmissiveMaterial(float strength, Texture* baseColor, ImageTexture* normalMap)
        : baseColor(baseColor), strength(strength), Material(normalMap) {
        emissive = strength > 0;
    }

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        attenuation = Vector3f::ZERO;
        scattered = Ray(Vector3f::ZERO, Vector3f::ZERO);

        return baseColor->getColor(hit.getU(), hit.getV()) * strength;
    }

    bool isEmissive() const override {
        return emissive;
    }

private:
    Texture* baseColor;
    float strength;
    bool emissive;
};

//
// Implemented independently
//
class TransmissiveMaterial : public Material {
public:
    explicit TransmissiveMaterial(Texture* baseColor, float refractiveIndex, ImageTexture* normalMap)
        : baseColor(baseColor), refractiveIndex(refractiveIndex), Material(normalMap) {}

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        Vector3f normal = getNormal(hit.getNormal(), hit.getU(), hit.getV());
        Vector3f diffuseColor = baseColor->getColor(hit.getU(), hit.getV());

        // Ideal Refraction
        Vector3f outwardNormal;
        float niOverNt;
        float cosine;
        float reflectProb;
        Vector3f refracted;
        Vector3f reflectDir = reflect(ray.getDirection(), normal);
        if (Vector3f::dot(ray.getDirection(), normal) > 0) {
            outwardNormal = -normal;
            niOverNt = refractiveIndex;
            cosine = refractiveIndex * Vector3f::dot(ray.getDirection(), normal) / ray.getDirection().length();
        } else {
            outwardNormal = normal;
            niOverNt = 1.0f / refractiveIndex;
            cosine = -Vector3f::dot(ray.getDirection(), normal) / ray.getDirection().length();
        }

        if (refract(ray.getDirection(), outwardNormal, niOverNt, refracted)) {
            reflectProb = schlick(cosine, refractiveIndex);
        } else {
            reflectProb = 1.0f;
        }

        if (rand01() < reflectProb) {
            Ray reflectRay(ray.pointAtParameter(hit.getT()) + reflectDir * rayEpsilon, reflectDir);

            attenuation = diffuseColor;
            scattered = reflectRay;
        } else {
            Ray refractRay(ray.pointAtParameter(hit.getT()) + refracted * rayEpsilon, refracted);

            attenuation = diffuseColor;
            scattered = refractRay;
        }

        return Vector3f::ZERO;
    }

private:
    Texture* baseColor;
    float refractiveIndex;
};

//
// Implemented independently
//
class SpecularMaterial : public Material {
public:
    explicit SpecularMaterial(Texture *specularMap, float fuzziness, ImageTexture* normalMap)
        : fuzziness(fuzziness), specularMap(specularMap), Material(normalMap) {}

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        Vector3f normal = getNormal(hit.getNormal(), hit.getU(), hit.getV());
        Vector3f specularColor = specularMap->getColor(hit.getU(), hit.getV());

        // Ideal Specular Reflection
        Vector3f reflectDir = reflect(ray.getDirection(), normal);
        reflectDir = reflectDir.normalized() + fuzziness * randomUnitVector3d();
        Ray reflectRay(ray.pointAtParameter(hit.getT()) + reflectDir * rayEpsilon, reflectDir.normalized());

        attenuation = specularColor;
        scattered = reflectRay;

        return Vector3f::ZERO;
    }

private:
    float fuzziness;
    Texture *specularMap;
};

//
// Implemented independently
//
class DiffuseMaterial : public Material {
public:
    explicit DiffuseMaterial(Texture* diffuseMap, ImageTexture* normalMap) : diffuseMap(diffuseMap), Material(normalMap) {}

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        Vector3f normal = getNormal(hit.getNormal(), hit.getU(), hit.getV());
        Vector3f diffuseColor = diffuseMap->getColor(hit.getU(), hit.getV());

        // Ideal Diffuse Reflection
        auto p0 = new objectPdf(lights, ray.pointAtParameter(hit.getT()));
        auto p1 = new cosinePdf(normal);
        mixturePdf mixed_pdf(p0, p1, 0.5);

        float pdf;
        Vector3f dir = mixed_pdf.generate(pdf);
        if (pdf <= 0) {
            attenuation = Vector3f::ZERO;
            scattered = Ray(Vector3f::ZERO, Vector3f::ZERO);
            return Vector3f::ZERO;
        }

        dir.normalize();
        Ray reflectRay(ray.pointAtParameter(hit.getT()) + dir * rayEpsilon, dir);
        float scatterPdf = getScatteringPdf(ray, normal, dir);

        float factor = scatterPdf / pdf;

        attenuation = 0.9 * diffuseColor * factor;
        scattered = reflectRay;

        return Vector3f::ZERO;
    }

private:
    Texture* diffuseMap;

    float getScatteringPdf(const Ray &ray, const Vector3f& normal, const Vector3f &scatteredDir) const {
        float cosine = Vector3f::dot(normal, scatteredDir);
        return cosine < 0 ? 0 : cosine / M_PI;
    }
};

//
// Referencing https://blog.csdn.net/u013412391/article/details/120597248
//
class GlossyMaterial : public Material {
public:
    explicit GlossyMaterial(Texture* albedo, Texture* roughness, Texture* metallic, ImageTexture* normalMap)
    : albedoMap(albedo), roughnessMap(roughness), metallicMap(metallic), Material(normalMap) {
    }


    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        Vector3f normal = getNormal(hit.getNormal(), hit.getU(), hit.getV());
        Vector3f albedo = albedoMap->getColor(hit.getU(), hit.getV());
        float roughness = roughnessMap->getColor(hit.getU(), hit.getV()).x();
        float metallic = metallicMap->getColor(hit.getU(), hit.getV()).y();

        const Vector3f dielectricF0 = Vector3f(0.04, 0.04, 0.04);
        Vector3f F0 = Vector3f::lerp(dielectricF0, albedo, metallic);

        Vector3f ggxNormal = sampleGGX(normal, roughness, rand01(), rand01());
        Vector3f m = orthonormalBasis(normal) * ggxNormal;

        Vector3f wo = -ray.getDirection();
        wo.normalized();

        Vector3f reflected = reflect(ray.getDirection(), m);

        float NdotV = Vector3f::dot(normal, wo);
        float NdotL = Vector3f::dot(normal, reflected);
        if (NdotL < 0) {
            attenuation = Vector3f::ZERO;
            scattered = Ray(Vector3f::ZERO, Vector3f::ZERO);
            return Vector3f::ZERO;
        }

        Vector3f fresnel = fresnelSchlick(Vector3f::dot(wo, normal), F0);

        float D = GGX_D(normal, m, roughness);
        float G = GGX_G1(wo, m, normal, roughness) * GGX_G1(reflected, m, normal, roughness);

        float pdf = GGX_Pdf(normal, m, wo, roughness);

        attenuation = albedo * fresnel * D * G / (4 * NdotV) / pdf;

        scattered = Ray(ray.pointAtParameter(hit.getT()) + reflected * rayEpsilon, reflected);
        return Vector3f::ZERO;
    }

    Texture* getAlbedoMap() const {
        return albedoMap;
    }

private:
    Texture* albedoMap;
    Texture* roughnessMap;
    Texture* metallicMap;

    static Vector3f fresnelSchlick(float cosTheta, const Vector3f &F0) {
        return F0 + (Vector3f(1, 1, 1) - F0) * pow(1 - cosTheta, 5);
    }

    static float chiPlus(float x) {
        return x > 0 ? 1 : 0;
    }

    // GGX Distribution
    // n: normal
    // m: normal of microfacet
    // alpha: roughness
    static float GGX_D(const Vector3f &n, const Vector3f &m, float alpha) {
        float cosTheta = Vector3f::dot(n, m);
        float tanTheta2 = (1 - cosTheta * cosTheta) / (cosTheta * cosTheta);
        float alpha2 = alpha * alpha;
        return chiPlus(cosTheta) * alpha2 / (M_PI * pow(cosTheta * cosTheta * (alpha2 + tanTheta2), 2));
    }

    // GGX Geometric Attenuation
    // v: view direction
    // m: normal of microfacet
    // n: normal
    // alpha: roughness
    static float GGX_G1(const Vector3f &v, const Vector3f &m, const Vector3f &n, float alpha) {
        float wdotm = Vector3f::dot(v, m);
        float wdotn = Vector3f::dot(v, n);
        float chi = chiPlus(wdotm / wdotn);
        float tanTheta2 = (1 - wdotm * wdotm) / (wdotm * wdotm);
        float alpha2 = alpha * alpha;
        return chi * 2 / (1 + sqrt(1 + alpha2 * tanTheta2));
    }

    static Vector3f sampleGGX(const Vector3f &n, float alpha, float u1, float u2) {
        float phi = 2 * M_PI * u1;
        float cosTheta = sqrt((1 - u2) / (1 + (alpha * alpha - 1) * u2));
        float sinTheta = sqrt(1 - cosTheta * cosTheta);
        return Vector3f(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    }

    static float GGX_Pdf(const Vector3f &n, const Vector3f &m, const Vector3f &v, float alpha) {
        float cosTheta = Vector3f::dot(n, m);
        float tanTheta2 = (1 - cosTheta * cosTheta) / (cosTheta * cosTheta);
        float alpha2 = alpha * alpha;
        return GGX_D(n, m, alpha) * cosTheta / (4 * Vector3f::dot(v, m));
    }
};

//
// Implemented independently
//
class MixedMaterial : public Material {
public:
    // ratio is the probability of using m1
    explicit MixedMaterial(Material *m1, Material *m2, float ratio) : m1(m1), m2(m2), ratio(ratio), Material(nullptr) {
        emissive = m1->isEmissive() || m2->isEmissive();
    }

    ~MixedMaterial() override {
        delete m1;
        delete m2;
    }

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        if (rand01() < ratio) {
            return m1->scatter(ray, hit, attenuation, scattered, lights);
        } else {
            return m2->scatter(ray, hit, attenuation, scattered, lights);
        }
    }

    bool isEmissive() const override {
        return emissive;
    }

private:
    Material *m1, *m2;
    float ratio;
    bool emissive;
};

//
// Implemented independently
//
class PrincipledSpecularMaterial : public Material {
public:
    explicit PrincipledSpecularMaterial(
            float specular, float transmission, float emission,
            float fuzziness, float refractiveIndex, float emissionStrength,
            Texture* diffuseMap, Texture* specularMap, Texture* emissionMap, ImageTexture* normalMap) : Material(normalMap) {
        auto *diffuseMaterial = new DiffuseMaterial(diffuseMap, normalMap);
        auto *specularMaterial = new SpecularMaterial(specularMap, fuzziness, normalMap);
        auto *transmissiveMaterial = new TransmissiveMaterial(diffuseMap, refractiveIndex, normalMap);
        auto *emissiveMaterial = new EmissiveMaterial(emissionStrength, emissionMap, normalMap);

        auto *mixedMaterial1 = new MixedMaterial(specularMaterial, diffuseMaterial, specular);
        auto *mixedMaterial2 = new MixedMaterial(transmissiveMaterial, mixedMaterial1, transmission);
        mixedMaterial = new MixedMaterial(emissiveMaterial, mixedMaterial2, emission);
    }

    ~PrincipledSpecularMaterial() override {
        delete mixedMaterial;
    }

    Vector3f scatter(const Ray &ray, const Hit &hit, Vector3f &attenuation, Ray &scattered, Object3D* lights) const override {
        return mixedMaterial->scatter(ray, hit, attenuation, scattered, lights);
    }

    bool isEmissive() const override {
        return mixedMaterial->isEmissive();
    }
private:
    MixedMaterial *mixedMaterial;
};

#endif // MATERIAL_H
