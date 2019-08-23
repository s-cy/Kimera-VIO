/* ----------------------------------------------------------------------------
 * Copyright 2017, Massachusetts Institute of Technology,
 * Cambridge, MA 02139
 * All Rights Reserved
 * Authors: Luca Carlone, et al. (see THANKS for the full author list)
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file   Logger.h
 * @brief  Logging output information.
 * @author Antoni Rosinol, Luca Carlone
 */

#pragma once

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "datasource/ETH_parser.h"  // REMOVE THIS!!

namespace VIO {

// Wrapper for std::ofstream to open/close it when created/destructed.
class OfstreamWrapper {
 public:
  OfstreamWrapper(const std::string& filename,
                  const bool& open_file_in_append_mode = false);
  ~OfstreamWrapper();

 public:
  std::ofstream ofstream_;

 private:
  void openLogFile(const std::string& output_file_name,
                   bool open_file_in_append_mode = false);

 private:
  const std::string filename_;
  const std::string output_path_;
  const bool open_file_in_append_mode = false;
};

class BackendLogger {
 public:
  BackendLogger();
  ~BackendLogger() = default;

  void logBackendOutput(const VioBackEndOutputPayload& output);
  void displayInitialStateVioInfo(const gtsam::Vector3& n_gravity_,
                                  const gtsam::Pose3& W_Pose_B_Lkf,
                                  const gtNavState& initial_state_gt,
                                  const ImuAccGyrS& imu_accgyr,
                                  const Timestamp& timestamp_k) const;

 private:
  void logBackendResultsCSV(const VioBackEndOutputPayload& output);
  void logSmartFactorsStats(const VioBackEndOutputPayload& output);
  void logBackendFactorsStats(const VioBackEndOutputPayload& output);
  void logBackendTiming(const VioBackEndOutputPayload& output);

 private:
  // Filenames to be saved in the output folder.
  OfstreamWrapper output_poses_vio_csv_;
  OfstreamWrapper output_smart_factors_stats_csv_;
  OfstreamWrapper output_backend_factors_stats_csv_;
  OfstreamWrapper output_backend_timing_csv_;

  gtsam::Pose3 W_Pose_Bprevkf_vio_;
  double timing_loggerBackend_;
};

class FrontendLogger {
 public:
  FrontendLogger();
  ~FrontendLogger() = default;

  void logFrontendResults(const TrackerStatusSummary& tracker_summary,
                          const size_t& nrKeypoints);

 private:
  // Filenames to be saved in the output folder.
  OfstreamWrapper output_frontend_;
};

class VisualizerLogger {
 public:
  VisualizerLogger();
  ~VisualizerLogger() = default;

  void logLandmarks(const PointsWithId& lmks);
  void logLandmarks(const cv::Mat& lmks);
  void logMesh(const cv::Mat& lmks,
               const cv::Mat& colors,
               const cv::Mat& mesh,
               const double& timestamp,
               bool log_accumulated_mesh = false);

 private:
  // Filenames to be saved in the output folder.
  OfstreamWrapper output_mesh_;
  OfstreamWrapper output_landmarks_;
};

class PipelineLogger {
 public:
  PipelineLogger();
  ~PipelineLogger() = default;

  void logPipelineOverallTiming(const std::chrono::milliseconds& duration);

 private:
  // Filenames to be saved in the output folder.
  OfstreamWrapper output_pipeline_timing_;
};

}  // namespace VIO
