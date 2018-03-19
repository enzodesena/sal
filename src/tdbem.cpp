/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#include "tdbem.h"
#include "salconstants.h"

namespace sal {
  
using sal::Source;
using sal::Microphone;
using sal::Time;
using sal::Length;
using sal::Sample;
using sal::UInt;
using mcl::Point;
using sal::Int;
  
  
TdBem::TdBem(Room* const room,
             Source* const source,
             Microphone* const microphone,
             const Time sampling_frequency,
             const Length spatial_sampling_period,
             const Sample specific_acoustic_impedance) :
        room_(room), source_(source), microphone_(microphone),
        sampling_frequency_(sampling_frequency),
        spatial_sampling_period_(spatial_sampling_period),
        specific_acoustic_impedance_(specific_acoustic_impedance),
        log_(false),
        source_delay_line_(sal::DelayFilter((UInt) ceil(room->max_distance()*sampling_frequency),
                                            (UInt) ceil(room->max_distance()*sampling_frequency))) {
  
  CalculatePoints();
  
  num_elements_ = points_.size();
          
  std::vector<sal::Length> all_distances;
  // Precalculate distances
  distance_los_ = Distance(microphone_->position(),
                                  source_->position());
  for (UInt i = 0; i<num_elements_; ++i) {
    distances_source_.push_back(mcl::Distance(points_[i],
                                                     source_->position()));
    all_distances.push_back(distances_source_[i]);
    distances_mic_.push_back(mcl::Distance(points_[i],
                                                  microphone_->position()));
    all_distances.push_back(distances_mic_[i]);
    
    // Precalculate distances between points
    distances_.push_back(std::vector<sal::Length>(num_elements_, 0.0));
    for (UInt j = 0; j<num_elements_; ++j) {
      if (i == j) { continue; }
      Length distance = mcl::Distance(points_[i], points_[j]);
      distances_[i][j] = distance;
      all_distances.push_back(distance);
    }
  }
          
          
  std::vector<sal::Sample> all_weights;
  
  for (UInt i = 0; i<num_elements_; ++i) {
    weights_current_.push_back(std::vector<sal::Sample>(num_elements_, 0.0));
    weights_previous_.push_back(std::vector<sal::Sample>(num_elements_, 0.0));
    for (UInt j = 0; j<num_elements_; ++j) {
      if (i == j) { continue; }
      sal::Sample dr_dn = CalculateDrDn(points_[i], points_[j], normal_vectors_[j]);
      
      Sample weight_current = CalculateWeightCurrent(dr_dn, distances_[i][j],
                                                     SOUND_SPEED,
                                                     sampling_frequency,
                                                     spatial_sampling_period,
                                                     specific_acoustic_impedance);
      weights_current_[i][j] = weight_current;
      all_weights.push_back(weight_current);
      
      Sample weight_previous = CalculateWeightPrevious(dr_dn, distances_[i][j],
                                                       SOUND_SPEED,
                                                       sampling_frequency,
                                                       spatial_sampling_period,
                                                       specific_acoustic_impedance);
      weights_previous_[i][j] = weight_previous;
      all_weights.push_back(weight_previous);
    }
    
    
    sal::Sample dr_dn = CalculateDrDn(microphone_->position(), points_[i],
                                      normal_vectors_[i]);
    weights_mic_current_.push_back(CalculateWeightCurrent(dr_dn, distances_mic_[i],
                                                     SOUND_SPEED,
                                                     sampling_frequency,
                                                     spatial_sampling_period,
                                                     specific_acoustic_impedance));
    weights_mic_previous_.push_back(CalculateWeightPrevious(dr_dn, distances_mic_[i],
                                                       SOUND_SPEED,
                                                       sampling_frequency,
                                                       spatial_sampling_period,
                                                       specific_acoustic_impedance));
    
    weights_source_.push_back(1.0/distances_source_[i]);
  }
          
          

          
          
  // Initialise delay lines
  for (UInt i = 0; i<num_elements_; ++i) {
    pressures_.push_back(sal::DelayFilter((UInt) ceil(room->max_distance()*sampling_frequency),
                                          (UInt) ceil(room->max_distance()*sampling_frequency)));
  }
          
  if (true) {
    Length min_distance = mcl::Min(all_distances);
    std::cout<<"Spatial sampling period: "<<spatial_sampling_period<<std::endl;
    std::cout<<"Minimum distance: "<<min_distance<<std::endl;
    
    std::cout<<"Minimum weight: "<<mcl::Min(all_weights)<<std::endl;
    std::cout<<"Maximum weight: "<<mcl::Max(all_weights)<<std::endl;
  }
}

Sample TdBem::CalculateWeightCurrent(Sample dr_dn, Length distance,
                                     Length sound_speed,
                                     Time sampling_frequency,
                                     Length spatial_sampling_period,
                                     Sample specific_acoustic_impedance) {
  return (dr_dn/pow(distance, 2.0) +
          dr_dn/(distance*sound_speed/sampling_frequency) +
          1.0/(distance*sound_speed/sampling_frequency*specific_acoustic_impedance))*
          pow(spatial_sampling_period, 2.0);
}

Sample TdBem::CalculateWeightPrevious(Sample dr_dn, Length distance,
                                      Length sound_speed,
                                      Time sampling_frequency,
                                      Length spatial_sampling_period,
                                      Sample specific_acoustic_impedance) {
  return -(dr_dn - 1.0/specific_acoustic_impedance) /
          (distance*sound_speed/sampling_frequency)*
          pow(spatial_sampling_period, 2.0);
}
  
Sample TdBem::CalculateDrDn(Point point_x, Point point_y, Point normal_y) {
  return mcl::DotProduct(mcl::Subtract(point_y, point_x),
                                normal_y) / Distance(point_y, point_x);
}

void TdBem::CalculatePoints() {
  // TODO: implement for other types of room
  assert(dynamic_cast<CuboidRoom*>(room_) != 0);
  Length room_x = ((CuboidRoom*) room_)->x();
  Length room_y = ((CuboidRoom*) room_)->y();
  Length room_z = ((CuboidRoom*) room_)->z();
  
  std::vector<mcl::Point> boundary_points;
  sal::Length sp = spatial_sampling_period_;
  
  // Points on surface parallel to xy plane
  for (UInt i=0; i<(floor(room_x/spatial_sampling_period_)-1); ++i) {
    for (UInt j=0; j<(floor(room_y/spatial_sampling_period_)-1); ++j) {
      // Points on z1 plane
      points_.push_back(Point((sal::Length)i * sp + sp,
                              (sal::Length)j * sp + sp,
                              0.0));
      normal_vectors_.push_back(Point(0.0, 0.0, 1.0));
      
      // Points on z2 plane
      points_.push_back(Point((sal::Length)i * sp + sp,
                              (sal::Length)j * sp + sp,
                              room_z));
      normal_vectors_.push_back(Point(0.0, 0.0, -1.0));
    }
  }
  
  // Points on surface parallel to xz plane
  for (UInt i=0; i<(floor(room_x/spatial_sampling_period_)-1); ++i) {
    for (UInt k=0; k<(floor(room_z/spatial_sampling_period_)-1); ++k) {
      // Points on y1 plane
      points_.push_back(Point((sal::Length)i * sp + sp,
                              0.0,
                              (sal::Length)k * sp + sp));
      normal_vectors_.push_back(Point(0.0, 1.0, 0.0));
      
      // Points on y2 plane
      points_.push_back(Point((sal::Length)i * sp + sp,
                              room_y,
                              (sal::Length)k * sp + sp));
      normal_vectors_.push_back(Point(0.0, -1.0, 0.0));
    }
  }
  // Points on surface parallel to yz plane
  for (UInt j=0; j<(floor(room_y/spatial_sampling_period_)-1); ++j) {
    for (UInt k=0; k<(floor(room_z/spatial_sampling_period_)-1); ++k) {
      // Points on x1 plane
      points_.push_back(Point(0.0,
                              (sal::Length)j * sp + sp,
                              (sal::Length)k * sp + sp));
      normal_vectors_.push_back(Point(1.0, 0.0, 0.0));
      
      // Points on x2 plane
      points_.push_back(Point(room_x,
                              (sal::Length)j * sp + sp,
                              (sal::Length)k * sp + sp));
      normal_vectors_.push_back(Point(-1.0, 0.0, 0.0));
    }
  }
}
  
  
void TdBem::Run() {
  mcl::Matrix<sal::Sample> boundary_pressure(0,0);
  if (log_) {
    boundary_pressure = mcl::Matrix<sal::Sample>(num_elements_,
                                                 source_->stream()->size());
  }
  
  UInt k=0;
  while (! source_->stream()->IsEmpty()) {
    if (log_) { std::cout<<"Running TDBEM sample n. "<<k<<std::endl; }
    source_delay_line_.Write(source_->stream()->Pull());
    
    
    for (UInt i = 0; i<num_elements_; ++i) {
      sal::Sample pressure = 0.0;
      // Line of sight to boundary point
      pressure += source_delay_line_.FractionalRead(distances_source_[i]*
                                                    sampling_frequency_/
                                                    SOUND_SPEED)
                  * weights_source_[i];
      
      for (UInt j = 0; j<num_elements_; ++j) {
        if (i == j) { continue; }
        Time delay = distances_[i][j] * sampling_frequency_ / SOUND_SPEED;
        pressure += (pressures_[j].FractionalRead(delay) * weights_current_[i][j] +
                     pressures_[j].FractionalRead(delay + 1.0) * weights_previous_[i][j])
                    / (2.0*PI);
      }
      pressures_[i].Write(pressure);
      
      if (log_) {
        boundary_pressure.set_element(i, k, pressure);
      }
    }
    
    // Extract pressure
    for (UInt i = 0; i<num_elements_; ++i) {
      Time delay = distances_mic_[i] * sampling_frequency_ / SOUND_SPEED;
      Sample pressure = (weights_mic_current_[i] * pressures_[i].FractionalRead(delay) +
                weights_mic_previous_[i] * pressures_[i].FractionalRead(delay+1.0))
                / (4.0*PI);
      
      microphone_->RecordPlaneWave(pressure,
                                   points_[i], i);
    }
    
    Time los_delay = distance_los_ * sampling_frequency_ / SOUND_SPEED;
    microphone_->RecordPlaneWave(source_delay_line_.FractionalRead(los_delay)
                                 / (4.0*PI*distance_los_),
                                 source_->position(),
                                 num_elements_);
    
    // Next time tick
    source_delay_line_.Tick();
    microphone_->Tick();
    for (UInt i = 0; i<num_elements_; ++i) {
      pressures_[i].Tick();
    }
    k++;
  }
  
  if (log_) { boundary_pressure.Save(log_file_name_, 10); }
}
  
void TdBem::WriteOutPoints(const std::string file_name) {
  mcl::Matrix<sal::Length> output(num_elements_, 6);
  for (UInt i=0; i<num_elements_; ++i) {
    output.set_element(i, 0, points_[i].x());
    output.set_element(i, 1, points_[i].y());
    output.set_element(i, 2, points_[i].z());
    output.set_element(i, 3, normal_vectors_[i].x());
    output.set_element(i, 4, normal_vectors_[i].y());
    output.set_element(i, 5, normal_vectors_[i].z());
  }
  output.Save(file_name);
}
  

} // namespace sal
