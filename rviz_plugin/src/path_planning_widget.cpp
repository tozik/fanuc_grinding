#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QCheckBox>
#include <QFuture>
#include <QtConcurrentRun>

#include "path_planning_widget.h"

fanuc_grinding_rviz_plugin::PathPlanningWidget::PathPlanningWidget(QWidget* parent) : QWidget(parent)
{
  setObjectName("PathPlanningWidget_");
  QLabel* surfacing_mode_label = new QLabel("Surfacing mode ");
  surfacing_mode_ = new QCheckBox;
  surfacing_mode_->setChecked(false);
  QHBoxLayout* surfacing_mode_layout = new QHBoxLayout;
  surfacing_mode_layout->addWidget(surfacing_mode_label);
  surfacing_mode_layout->addWidget(surfacing_mode_);

  depth_of_pass_label_ = new QLabel("Depth of pass");
  depth_of_pass_ = new QDoubleSpinBox;
  depth_of_pass_->setSuffix(" mm");
  depth_of_pass_->setValue(1.0);
  depth_of_pass_->setDecimals(3);
  depth_of_pass_->setRange(0.001, 100);
  QHBoxLayout* depth_of_pass_layout = new QHBoxLayout;
  depth_of_pass_layout->addWidget(depth_of_pass_label_);
  depth_of_pass_layout->addWidget(depth_of_pass_);

  QLabel* covering_percentage_label = new QLabel("Covering percentage:");
  covering_percentage_ = new QSpinBox;
  covering_percentage_->setSuffix("%");
  covering_percentage_->setValue(40);
  covering_percentage_->setRange(0, 99);
  QHBoxLayout* covering_percentage_layout = new QHBoxLayout;
  covering_percentage_layout->addWidget(covering_percentage_label);
  covering_percentage_layout->addWidget(covering_percentage_);

  QLabel* extrication_frequency_label = new QLabel("Extrication frequency:");
  extrication_frequency_ = new QSpinBox;
  extrication_frequency_->setValue(5);
  extrication_frequency_->setRange(0, 50);
  QHBoxLayout* extrication_frequency_layout = new QHBoxLayout;
  extrication_frequency_layout->addWidget(extrication_frequency_label);
  extrication_frequency_layout->addWidget(extrication_frequency_);

  QLabel* extrication_coefficient_label = new QLabel("Extrication coefficient:");
  extrication_coefficient_ = new QSpinBox;
  extrication_coefficient_->setValue(5);
  extrication_coefficient_->setRange(1, 20);
  QHBoxLayout* extrication_coefficient_layout = new QHBoxLayout;
  extrication_coefficient_layout->addWidget(extrication_coefficient_label);
  extrication_coefficient_layout->addWidget(extrication_coefficient_);

  QLabel* grind_diameter_label = new QLabel("Grinder diameter:");
  grind_diameter_ = new QDoubleSpinBox;
  grind_diameter_->setSuffix(" mm");
  grind_diameter_->setValue(30);
  grind_diameter_->setDecimals(1);
  grind_diameter_->setRange(1.0, 200.0);
  QHBoxLayout* grind_diameter_layout = new QHBoxLayout;
  grind_diameter_layout->addWidget(grind_diameter_label);
  grind_diameter_layout->addWidget(grind_diameter_);

  QLabel* lean_angle_axis_label = new QLabel("Axis of rotation:");
  lean_angle_axis_x_ = new QRadioButton("x");
  lean_angle_axis_y_ = new QRadioButton("y");
  lean_angle_axis_z_ = new QRadioButton("z");
  QHBoxLayout* lean_angle_axis_layout = new QHBoxLayout;
  lean_angle_axis_layout->addWidget(lean_angle_axis_label);
  lean_angle_axis_layout->addStretch(1);
  lean_angle_axis_layout->addWidget(lean_angle_axis_x_);
  lean_angle_axis_layout->addWidget(lean_angle_axis_y_);
  lean_angle_axis_layout->addWidget(lean_angle_axis_z_);
  lean_angle_axis_layout->addStretch(1);

  QLabel* angle_value_label = new QLabel("Angle value:");
  angle_value_ = new QDoubleSpinBox;
  angle_value_->setSuffix(" degrees");
  angle_value_->setRange(-90, 90);
  QHBoxLayout* angle_value_layout = new QHBoxLayout;
  angle_value_layout->addWidget(angle_value_label);
  angle_value_layout->addWidget(angle_value_);

  compute_trajectory_ = new QPushButton("Compute trajectory");
  compute_trajectory_->setMinimumHeight(90);

  execute_trajectory_ = new QPushButton("Execute trajectory");
  execute_trajectory_->setEnabled(false);
  execute_trajectory_->setMinimumHeight(60);
  visualize_trajectory_ = new QPushButton("Visualize trajectory");
  visualize_trajectory_->setEnabled(false);
  visualize_trajectory_->setMinimumHeight(60);
  QHBoxLayout* button_path_planning_layout = new QHBoxLayout;
  button_path_planning_layout->addWidget(visualize_trajectory_);
  button_path_planning_layout->addWidget(execute_trajectory_);

  QVBoxLayout* path_planning_layout = new QVBoxLayout(this);
  path_planning_layout->addLayout(surfacing_mode_layout);
  path_planning_layout->addLayout(depth_of_pass_layout);
  path_planning_layout->addLayout(covering_percentage_layout);
  path_planning_layout->addLayout(extrication_frequency_layout);
  path_planning_layout->addLayout(extrication_coefficient_layout);
  path_planning_layout->addLayout(grind_diameter_layout);
  path_planning_layout->addLayout(lean_angle_axis_layout);
  path_planning_layout->addLayout(angle_value_layout);
  path_planning_layout->addStretch(2);
  path_planning_layout->addWidget(compute_trajectory_);
  path_planning_layout->addStretch(1);
  path_planning_layout->addLayout(button_path_planning_layout);
  path_planning_layout->addStretch(8);

  //Connect handlers
  // At each modification of the widget, we call triggerSave
  connect(surfacing_mode_, SIGNAL(stateChanged(int)), this, SLOT(setDepthOfPassEnable(int)));
  connect(surfacing_mode_, SIGNAL(stateChanged(int)), this, SLOT(triggerSave()));
  connect(covering_percentage_, SIGNAL(valueChanged(int)), this, SLOT(triggerSave()));
  connect(extrication_frequency_, SIGNAL(valueChanged(int)), this, SLOT(triggerSave()));
  connect(extrication_coefficient_, SIGNAL(valueChanged(int)), this, SLOT(triggerSave()));
  connect(grind_diameter_, SIGNAL(valueChanged(double)), this, SLOT(triggerSave()));
  connect(depth_of_pass_, SIGNAL(valueChanged(double)), this, SLOT(triggerSave()));
  connect(lean_angle_axis_x_, SIGNAL(clicked()), this, SLOT(triggerSave()));
  connect(lean_angle_axis_y_, SIGNAL(clicked()), this, SLOT(triggerSave()));
  connect(lean_angle_axis_z_, SIGNAL(clicked()), this, SLOT(triggerSave()));
  connect(angle_value_, SIGNAL(valueChanged(double)), this, SLOT(triggerSave()));

  // Enable or disable compute_trajectory_button_
  connect(this, SIGNAL(enableComputeTrajectoryButton(bool)), this, SLOT(enableComputeTrajectoryButtonHandler(bool)));

  // connect each buttons to different functions
  connect(compute_trajectory_, SIGNAL(released()), this, SLOT(computeTrajectoryButtonHandler()));
  connect(visualize_trajectory_, SIGNAL(released()), this, SLOT(visualizeTrajectoryButtonHandler()));
  connect(execute_trajectory_, SIGNAL(released()), this, SLOT(executeTrajectoryButtonHandler()));
  connect(this, SIGNAL(enableVizSimButton()), this, SLOT(enableVizSimButtonHandler()));

  // Subscriber to receive messages from the exterior
  status_sub_ = nh_.subscribe("path_planning_status", 1, &fanuc_grinding_rviz_plugin::PathPlanningWidget::newStatusMessage, this);

  // Setup client
  path_planning_service_ = nh_.serviceClient<fanuc_grinding_path_planning::PathPlanningService>("path_planning_service");

  QFuture<void> future = QtConcurrent::run(this, &fanuc_grinding_rviz_plugin::PathPlanningWidget::connectToServices);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::newStatusMessage(const std_msgs::String::ConstPtr &msg)
{
  Q_EMIT sendStatus(QString::fromStdString(msg->data));
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::enableComputeTrajectoryButtonHandler(bool state)
{
  compute_trajectory_->setEnabled(state);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::setPathPlanningParams(fanuc_grinding_path_planning::PathPlanningService::Request &params)
{
  srv_path_planning_.request.SurfacingMode = params.SurfacingMode;
  srv_path_planning_.request.CoveringPercentage = params.CoveringPercentage;
  srv_path_planning_.request.ExtricationFrequency = params.ExtricationFrequency;
  srv_path_planning_.request.ExtricationCoefficient = params.ExtricationCoefficient;
  srv_path_planning_.request.GrindDiameter = params.GrindDiameter;
  srv_path_planning_.request.DepthOfPath = params.DepthOfPath;
  srv_path_planning_.request.AngleX = params.AngleX;
  srv_path_planning_.request.AngleY = params.AngleY;
  srv_path_planning_.request.AngleZ = params.AngleZ;
  srv_path_planning_.request.AngleValue = params.AngleValue;
  updateGUI();
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::updateGUI()
{
  surfacing_mode_->setChecked(srv_path_planning_.request.SurfacingMode);
  covering_percentage_->setValue(srv_path_planning_.request.CoveringPercentage);
  extrication_frequency_->setValue(srv_path_planning_.request.ExtricationFrequency);
  extrication_coefficient_->setValue(srv_path_planning_.request.ExtricationCoefficient);
  grind_diameter_->setValue(srv_path_planning_.request.GrindDiameter * 1000.0); // meters to millimeters
  depth_of_pass_->setValue(srv_path_planning_.request.DepthOfPath * 1000.0); // meters to millimeters
  lean_angle_axis_x_->setChecked(srv_path_planning_.request.AngleX);
  lean_angle_axis_y_->setChecked(srv_path_planning_.request.AngleY);
  lean_angle_axis_z_->setChecked(srv_path_planning_.request.AngleZ);
  angle_value_->setValue(srv_path_planning_.request.AngleValue * 360.0 / M_PI); // radians to degrees
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::updateInternalValues()
{
  srv_path_planning_.request.SurfacingMode = surfacing_mode_->isChecked();
  srv_path_planning_.request.CoveringPercentage = covering_percentage_->value();
  srv_path_planning_.request.ExtricationFrequency = extrication_frequency_->value();
  srv_path_planning_.request.ExtricationCoefficient = extrication_coefficient_->value();
  srv_path_planning_.request.GrindDiameter = grind_diameter_->value() / 1000.0; // millimeters to meters
  srv_path_planning_.request.DepthOfPath = depth_of_pass_->value() / 1000.0; // millimeters to meters
  srv_path_planning_.request.AngleX = lean_angle_axis_x_->isChecked();
  srv_path_planning_.request.AngleY = lean_angle_axis_y_->isChecked();
  srv_path_planning_.request.AngleZ = lean_angle_axis_z_->isChecked();
  srv_path_planning_.request.AngleValue = angle_value_->value() / 360.0 * M_PI; // degrees to radians
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::setDepthOfPassEnable(const int state)
{
  depth_of_pass_->setEnabled(!state);
  depth_of_pass_label_->setEnabled(!state);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::setCADAndScanParams(const QString cad_filename,
                                                                         const QString cad_marker_name,
                                                                         const QString scan_filename,
                                                                         const QString scan_marker_name)
{
  srv_path_planning_.request.CADFileName = cad_filename.toStdString();
  srv_path_planning_.request.CADMarkerName = cad_marker_name.toStdString();
  srv_path_planning_.request.ScanFileName = scan_filename.toStdString();
  srv_path_planning_.request.ScanMarkerName = scan_marker_name.toStdString();
}

std::vector<geometry_msgs::Pose> fanuc_grinding_rviz_plugin::PathPlanningWidget::getRobotPoses()
{
  return srv_path_planning_.response.RobotPosesOutput;
}

std::vector<bool> fanuc_grinding_rviz_plugin::PathPlanningWidget::getPointColorViz()
{
  // TODO: Can we avoid copying/duplicating?
  std::vector<bool> temp;
  for(std::vector<uint8_t>::iterator iter (srv_path_planning_.response.PointColorVizOutput.begin());
      iter != srv_path_planning_.response.PointColorVizOutput.end();
      ++iter)
  {
    temp.push_back(*iter);
  }

  return temp;
}

std::vector<int> fanuc_grinding_rviz_plugin::PathPlanningWidget::getIndexVector()
{
  return srv_path_planning_.response.IndexVectorOutput;
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::computeTrajectoryButtonHandler()
{
  // Fill service parameters with GUI values
  updateInternalValues();
  // get CAD and Scan params which are stored in grinding rviz plugin
  Q_EMIT getCADAndScanParams();

  // Fill in the request
  srv_path_planning_.request.Compute = true;
  srv_path_planning_.request.Visualization = false;
  srv_path_planning_.request.Simulation = false;

  QFuture<void> future = QtConcurrent::run(this, &PathPlanningWidget::pathPlanningService);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::enableVizSimButtonHandler()
{
  if(visualize_trajectory_->isEnabled() == false)
  {
    visualize_trajectory_->setEnabled(true);
  }
  if(execute_trajectory_->isEnabled() == false)
  {
    execute_trajectory_->setEnabled(true);
  }
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::visualizeTrajectoryButtonHandler()
{
  // If GUI has been changed, compute_trajectory_button_ is enabled.
  // So the pose is not up-to-date with GUI values
  // We inform the user that is an old pose
  if(compute_trajectory_->isEnabled() == true)
  {
    QMessageBox msgBox;
    msgBox.setText("Values have been modified");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setInformativeText("If you continue, you will deal with an old trajectory");
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if(msgBox.exec() == QMessageBox::Abort)
        return;
  }

  // Get CAD and Scan params which are stored in grinding rviz plugin
  Q_EMIT getCADAndScanParams();

  // Fill in the request
  srv_path_planning_.request.Compute = false;
  srv_path_planning_.request.Visualization = true;
  srv_path_planning_.request.Simulation = false;
  for (unsigned int i = 0; i < srv_path_planning_.response.RobotPosesOutput.size(); ++i)
  {
    // NB: RobotPoses and PointColorViz have same size
    srv_path_planning_.request.RobotPosesInput.push_back(srv_path_planning_.response.RobotPosesOutput[i]);
    srv_path_planning_.request.PointColorVizInput.push_back(srv_path_planning_.response.PointColorVizOutput[i]);
  }
  for (unsigned int j = 0; j < srv_path_planning_.response.IndexVectorOutput.size(); ++j)
  {
    srv_path_planning_.request.IndexVectorInput.push_back(srv_path_planning_.response.IndexVectorOutput[j]);
  }

  // Start client service call in an other thread
  QFuture<void> future = QtConcurrent::run(this, &PathPlanningWidget::pathPlanningService);

}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::executeTrajectoryButtonHandler()
{
  // If GUI has been changed, compute_trajectory_button_ is enable.
  // So, the pose is not up-to-date with GUI values
  // We inform the user that is an old pose
  if(compute_trajectory_->isEnabled() == true)
  {
    QMessageBox msgBox;
    msgBox.setText("Values have been modified");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setInformativeText("If you continue, you will deal with an old trajectory");
    msgBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if(msgBox.exec() == QMessageBox::Abort)
      return;
  }

  // get CAD and Scan params which are stored in grinding rviz plugin
  Q_EMIT getCADAndScanParams();

  // Fill in the request
  srv_path_planning_.request.Compute = false;
  srv_path_planning_.request.Visualization = false;
  srv_path_planning_.request.Simulation = true;
  for (unsigned int i = 0; i < srv_path_planning_.response.RobotPosesOutput.size(); ++i)
  {
    // NB: RobotPoses and PointColorViz have same size
    srv_path_planning_.request.RobotPosesInput.push_back(srv_path_planning_.response.RobotPosesOutput[i]);
    srv_path_planning_.request.PointColorVizInput.push_back(srv_path_planning_.response.PointColorVizOutput[i]);
  }
  for (unsigned int j = 0; j < srv_path_planning_.response.IndexVectorOutput.size(); ++j)
  {
    srv_path_planning_.request.IndexVectorInput.push_back(srv_path_planning_.response.IndexVectorOutput[j]);
  }
  //srv_.request.*request* = *value*;
  // Start client service call in an other thread
  QFuture<void> future = QtConcurrent::run(this, &PathPlanningWidget::pathPlanningService);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::pathPlanningService()
{
  if(srv_path_planning_.request.Compute)
    Q_EMIT enableComputeTrajectoryButton(false);

  // Disable UI
  Q_EMIT enablePanel(false);

  // Call client service
  path_planning_service_.call(srv_path_planning_);
  Q_EMIT sendStatus(QString::fromStdString(srv_path_planning_.response.ReturnMessage));

  if(srv_path_planning_.response.ReturnStatus == true)
  {
    if (srv_path_planning_.request.Compute)
    {
      // If visualization and simulation buttons are disabled, we put them to an enable state
      Q_EMIT enableVizSimButton();
      Q_EMIT enablePanelPostProcessor();
    }
  }
  else
  {
    Q_EMIT sendMsgBox("Error in path planning service",
                      QString::fromStdString(srv_path_planning_.response.ReturnMessage), "");
    Q_EMIT enableComputeTrajectoryButton(true);
  }

  // Re-enable UI
  Q_EMIT enablePanel(true); // Enable UI
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::connectToServices()
{
  Q_EMIT enablePanel(false);

  // Check offset_move_robot_ connection
  Q_EMIT sendStatus("Connecting to service");
  while (ros::ok())
  {
    if (path_planning_service_.waitForExistence(ros::Duration(2)))
    {
      ROS_INFO_STREAM(objectName().toStdString() + " RViz panel connected to the service " << path_planning_service_.getService());
      Q_EMIT sendStatus(QString::fromStdString("RViz panel connected to the service: " + path_planning_service_.getService()));
      break;
    }
    else
    {
      ROS_WARN_STREAM(objectName().toStdString() + " RViz panel could not connect to ROS service:\n\t" << path_planning_service_.getService());
      Q_EMIT sendStatus(QString::fromStdString("RViz panel could not connect to ROS service: " + path_planning_service_.getService()));
      sleep(1);
    }
  }

  ROS_INFO_STREAM(objectName().toStdString() + " service connections have been made");
  Q_EMIT sendStatus("Ready to take commands");
  Q_EMIT enablePanel(true);
}

void fanuc_grinding_rviz_plugin::PathPlanningWidget::triggerSave()
{
  Q_EMIT guiChanged();
  updateInternalValues();
  updateGUI();
  Q_EMIT enableComputeTrajectoryButton(true);
}

// Save all configuration data from this panel to the given Config object
void fanuc_grinding_rviz_plugin::PathPlanningWidget::save(rviz::Config config)
{
  // Save offset value into the config file
  config.mapSetValue(objectName() + "surfacing_mode", surfacing_mode_->isChecked());
  config.mapSetValue(objectName() + "covering_percentage", covering_percentage_->value());
  config.mapSetValue(objectName() + "extrication_frequency", extrication_frequency_->value());
  config.mapSetValue(objectName() + "extrication_coefficient", extrication_coefficient_->value());
  config.mapSetValue(objectName() + "grind_diameter", grind_diameter_->value());
  config.mapSetValue(objectName() + "depth_of_path", depth_of_pass_->value());
  config.mapSetValue(objectName() + "radio_x", lean_angle_axis_x_->isChecked());
  config.mapSetValue(objectName() + "radio_y", lean_angle_axis_y_->isChecked());
  config.mapSetValue(objectName() + "radio_z", lean_angle_axis_z_->isChecked());
  config.mapSetValue(objectName() + "angle_value", angle_value_->value());
}

// Load all configuration data for this panel from the given Config object.
void fanuc_grinding_rviz_plugin::PathPlanningWidget::load(const rviz::Config& config)
{
  int tmp_int;
  bool tmp_bool;
  float tmp_float;
  QString tmp_string;
  // Load offset value from config file (if it exists)
  if (config.mapGetBool(objectName() + "surfacing_mode", &tmp_bool))
    surfacing_mode_->setChecked(tmp_bool);
  else
    surfacing_mode_->setChecked(false);
  if (config.mapGetInt(objectName() + "covering_percentage", &tmp_int))
    covering_percentage_->setValue(tmp_int);
  else
    covering_percentage_->setValue(40);

  if (config.mapGetInt(objectName() + "extrication_frequency", &tmp_int))
    extrication_frequency_->setValue(tmp_int);
  else
    extrication_coefficient_->setValue(3);

  if (config.mapGetInt(objectName() + "extrication_coefficient", &tmp_int))
    extrication_coefficient_->setValue(tmp_int);
  else
    extrication_coefficient_->setValue(3);

  if (config.mapGetInt(objectName() + "grind_diameter", &tmp_int))
    grind_diameter_->setValue(tmp_int);
  else
    grind_diameter_->setValue(120);

  if (config.mapGetInt(objectName() + "depth_of_path", &tmp_int))
    depth_of_pass_->setValue(tmp_int);
  else
    depth_of_pass_->setValue(1.0);

  if (config.mapGetBool(objectName() + "radio_x", &tmp_bool))
    lean_angle_axis_x_->setChecked(tmp_bool);
  if (config.mapGetBool(objectName() + "radio_y", &tmp_bool))
    lean_angle_axis_y_->setChecked(tmp_bool);
  if (config.mapGetBool(objectName() + "radio_z", &tmp_bool))
    lean_angle_axis_z_->setChecked(tmp_bool);
  if (!lean_angle_axis_x_->isChecked() && !lean_angle_axis_y_->isChecked() && !lean_angle_axis_z_->isChecked())
  {
    lean_angle_axis_z_->setChecked(true);
  }

  if (config.mapGetFloat(objectName() + "angle_value", &tmp_float))
    angle_value_->setValue(tmp_float);
  else
    angle_value_->setValue(10);
}
