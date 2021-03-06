/**
 * \file
 * \brief BeamSettings class declaration
 */
#ifndef BEAM_SETTINGS_H
#define BEAM_SETTINGS_H

#include "host_image_3d.cuh"
#include "float3_affine_transform.cuh"
#include "float3_idx_transform.cuh"
#include <vector>
//#include "float3_from_fan_transform.cuh"

/**
 * \brief ...
 */
class BeamSettings {
public:
    /**
     * @brief ...
     * @param spotWeights Non-owning pointer to ...
     * @param beamEnergies ...
     * @param spotSigmas ...
     * @param raySpacing ...
     * @param tracerSteps ...
     * @param sourceDist ...
     * @param spotIdxToGantry ...
     * @param gantryToImIdx ...
     * @param gantryToDoseIdx ...
     */
    BeamSettings(HostPinnedImage3D<float>* const spotWeights, const std::vector<float> beamEnergies, const std::vector<float2> spotSigmas, const float2 raySpacing, const unsigned int tracerSteps, const float2 sourceDist, const Float3IdxTransform spotIdxToGantry, const Float3AffineTransform gantryToImIdx, const Float3AffineTransform gantryToDoseIdx);

    /**
     * @brief ...
     * @return ...
     */
    HostPinnedImage3D<float>* getWeights();

    /**
     * @brief ...
     * @return ...
     */
    std::vector<float>& getEnergies();

    /**
     * @brief ...
     * @return ...
     */
    std::vector<float2>& getSpotSigmas();

    /**
     * @brief ...
     * @return ...
     */
    float2 getRaySpacing() const;

    /**
     * @brief ...
     * @return ...
     */
    unsigned int getSteps() const;

    /**
     * @brief ...
     * @return ...
     */
    float2 getSourceDist() const;

    /**
     * @brief ...
     * @return ...
     */
    Float3IdxTransform getSpotIdxToGantry() const;

    /**
     * @brief ...
     * @return ...
     */
    Float3AffineTransform getGantryToImIdx() const;

    /**
     * @brief ...
     * @return ...
     */
    Float3AffineTransform getGantryToDoseIdx() const;

    //
    // brief ...
    // return ...
    //
    //Float3FromFanTransform getFITII() const;

    //
    // brief ...
    // return ...
    //
    //Float3FromFanTransform getFITDI() const;

private:
    HostPinnedImage3D<float>* const sWghts;///< Non-owning pointer to...
    std::vector<float> bEnergies;   ///< ...
    std::vector<float2> sSigmas;    ///< ...
    float2 rSpacing;                ///< ...
    unsigned int steps;             ///< ...
    float2 sDist;                   ///< ...
    Float3IdxTransform sITG;        ///< ...
    Float3AffineTransform gTII;     ///< ...
    Float3AffineTransform gTDI;     ///< ...
    //Float3FromFanTransform fITII; ///< ...
    //Float3FromFanTransform fITDI; ///< ...
};

#endif // BEAM_SETTINGS_H
