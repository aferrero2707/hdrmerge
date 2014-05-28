/*
 *  HDRMerge - HDR exposure merging software.
 *  Copyright 2012 Javier Celaya
 *  jcelaya@gmail.com
 *
 *  This file is part of HDRMerge.
 *
 *  HDRMerge is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HDRMerge is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HDRMerge. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _IMAGESTACK_H_
#define _IMAGESTACK_H_

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include "Image.hpp"
#include "ProgressIndicator.hpp"
#include "EditableMask.hpp"
#include "LoadSaveOptions.hpp"

namespace hdrmerge {

class ImageStack {
public:
    ImageStack() : width(0), height(0) { setGamma(2.2f); }

    int load(const LoadOptions & options, ProgressIndicator & progress);
    int save(const SaveOptions & options, ProgressIndicator & progress);
    void align();
    void crop();
    void computeRelExposures();
    void generateMask() {
        mask.generateFrom(*this);
    }

    size_t size() const { return images.size(); }

    size_t getWidth() const {
        return width;
    }

    size_t getHeight() const {
        return height;
    }

    Image & getImage(unsigned int i) {
        return *images[i];
    }
    const Image & getImage(unsigned int i) const {
        return *images[i];
    }

    bool addImage(std::unique_ptr<Image> & i);
    std::string buildOutputFileName() const;
    double value(size_t x, size_t y) const;
    void compose(float * dst) const;
    void setGamma(float g);
    uint8_t toneMap(double v) {
        return toneCurve[(int)std::floor(v)];
    }
    int getBestLayerAt(size_t x, size_t y, int i) const {
        while (i < images.size() - 1 &&
            (!images[i]->contains(x, y) ||
            images[i]->isSaturated(x, y) ||
            images[i]->isSaturatedAround(x, y))) ++i;
        return i;
    }
    bool isValidAt(int layer, size_t x, size_t y) const {
        return images[layer]->contains(x, y);
    }

    uint8_t getImageAt(size_t x, size_t y) const {
        return mask.getImageAt(x, y);
    }
    void startEditAction(bool add, int layer) {
        mask.startAction(add, layer);
    }
    void editPixels(size_t x, size_t y, size_t radius) {
        mask.paintPixels(*this, x, y, radius);
    }
    EditableMask::Area undo() {
        return mask.undo();
    }
    EditableMask::Area redo() {
        return mask.redo();
    }

private:
    std::vector<std::unique_ptr<Image>> images;   ///< Images, from most to least exposed
    EditableMask mask;
    size_t width;
    size_t height;
    uint8_t toneCurve[65536];

    std::string replaceArguments(const std::string & maskFileName, const std::string & outFileName);
};

} // namespace hdrmerge

#endif // _IMAGESTACK_H_
