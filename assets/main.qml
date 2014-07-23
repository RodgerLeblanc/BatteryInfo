/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2

Page {
    ScrollView {
        Container {
            ToggleButton {
                id: uiToggle
                checked: false
                visible: false
            }
            Container {
            	id: m_uiConscious 
            	visible: uiToggle.checked            
            	objectName: "m_uiConscious"
            	Header {
                	title: "General"
                }
                Label {
                    id: m_batteryName
                    objectName: "m_batteryName"
                }
                Label {
                    id: m_temperature
                    objectName: "m_temperature"
                }
                Label {
                    id: m_batteryVoltage
                    objectName: "m_batteryVoltage"
                }
                Label {
                    id: m_stateOfHealt
                    objectName: "m_stateOfHealth"
                }
                Label {
                    id: m_stateOfCharge
                    objectName: "m_stateOfCharge"
                }
                Label {
                    id: m_timeToEmpty
                    objectName: "m_timeToEmpty"
                }
                Label {
                    id: m_designCapacity
                    objectName: "m_designCapacity"
                }
                Label {
                    id: m_maxSystemVoltage
                    objectName: "m_maxSystemVoltage"
                }
                Label {
                    id: m_minSystemVoltage
                    objectName: "m_minSystemVoltage"
                }
                Label {
                    id: m_averageCurrent
                    objectName: "m_averageCurrent"
                }
                Label {
                    id: m_cycleCount
                    objectName: "m_cycleCount"
                }
            } // end of uiConscious container
            Container {
                id: m_rawData
                objectName: "m_rawData"
                visible: !uiToggle.checked
                topPadding: 30
                bottomPadding: topPadding
                rightPadding: topPadding
                leftPadding: topPadding            
                Container {
                    bottomPadding: 30
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        text: "Enter battery size :"
                    }
                    TextField {
                        text: _app.getSettings("batterySize", "1800")
                        hintText: "ie: 4500"
                        onTextChanging: {
                            _app.saveSettings("batterySize", text)
                        }
                    }
                }
                Label {
                    objectName: "m_label"
                    multiline: true
                }
//                ScrollView {
                    Container {
                        maxHeight: 400
                        Label {
                            objectName: "m_label2"
                            multiline: true
                        }                    
                    }                    
//                }
            } // end of rawData container
        } // end of main container
    } // end of ScrollView
} // end of Page
