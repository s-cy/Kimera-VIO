/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   DisplayModule.cpp
 * @brief  Pipeline module to render/display 2D/3D visualization.
 * @author Antoni Rosinol
 */

#pragma once

#include <utility>

#include "kimera-vio/common/vio_types.h"
#include "kimera-vio/pipeline/PipelineModule.h"
#include "kimera-vio/utils/Macros.h"
#include "kimera-vio/visualizer/Display-definitions.h"
#include "kimera-vio/visualizer/Display.h"
#include "kimera-vio/visualizer/Visualizer3D-definitions.h"

namespace VIO {

/**
 * @brief The DisplayModule class receives a VisualizerOutput payload which
 * contains images to be displayed and uses a display to render it.
 * Since displaying must be done in the main thread, the
 * DisplayModule should spin in the main thread to avoid errors.
 */
class DisplayModule
    : public SISOPipelineModule<DisplayInputBase, NullPipelinePayload> {
 public:
  KIMERA_POINTER_TYPEDEFS(DisplayModule);
  KIMERA_DELETE_COPY_CONSTRUCTORS(DisplayModule);

  using SISO = SISOPipelineModule<DisplayInputBase, NullPipelinePayload>;

  DisplayModule(DisplayQueue* input_queue,
                OutputQueue* output_queue,
                bool parallel_run,
                DisplayBase::UniquePtr&& display)
      : SISO(input_queue, output_queue, "Display", parallel_run),
        display_(std::move(display)) {}

  virtual ~DisplayModule() = default;

  virtual OutputUniquePtr spinOnce(InputUniquePtr input) {
    CHECK(input);
    display_->spinOnce(std::move(input));
    return VIO::make_unique<NullPipelinePayload>();
  }

  typename MISO::InputUniquePtr getInputPacket() override {
    if (display_->display_type_ == DisplayType::kPangolin) {
      // If we are using pangolin just fake a constant input of messages
      // to not block the visualizer.
      return VIO::make_unique<DisplayInputBase>();
    }

    typename MISO::InputUniquePtr input = nullptr;
    bool queue_state = false;

    if (MISO::parallel_run_) {
      queue_state = input_queue_->popBlocking(input);
    } else {
      queue_state = input_queue_->pop(input);
    }

    if (queue_state) {
      return input;
    } else {
      LOG(WARNING) << "Module: " << MISO::name_id_ << " - "
                   << "Input queue: " << input_queue_->queue_id_
                   << " didn't return an output.";
      return nullptr;
    }
  }

 private:
  // The renderer used to display the visualizer output.
  DisplayBase::UniquePtr display_;
};

}  // namespace VIO
