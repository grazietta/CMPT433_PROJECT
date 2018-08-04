//
//  ViewController.swift
//  smileDetectorAgain
//
//  Created by Grazietta Hof on 2018-07-29.
//  Copyright © 2018 Grazietta Hof. All rights reserved.
//

import UIKit
import Firebase
import OpenTok
import VideoToolbox

class ViewController: UIViewController, VideoCaptureDelegate {
    
    var firebase: Firebase!
    var publisher: OTPublisher?
    var capturer: ExampleVideoCapture?
    var detector: VisionFaceDetector?
    var emotion: String!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        emotion = "frown"
        firebase = Firebase()
    
        //options taken from https://github.com/robjperez/SmileDetector/blob/master/SmileDetector/ViewController.swift
        let options = VisionFaceDetectorOptions()
        options.modeType = .fast
        options.landmarkType = .all
        options.classificationType = .all
        options.minFaceSize = CGFloat(0.2)
        options.isTrackingEnabled = true
        detector = Vision.vision().faceDetector(options: options)
        //***********************//
        
        capturer = ExampleVideoCapture()
        
        let pubSettings = OTPublisherSettings()
        pubSettings.videoCapture = capturer!
        capturer!.delegate = self
        publisher = OTPublisher(delegate: self, settings: pubSettings)
        
        if let videoView = publisher?.view {
            view.addSubview(videoView)
            videoView.frame = view.bounds
        }
    }
    
     func frameCaptured(frame: CVPixelBuffer, orientation: OTVideoOrientation) {
        
        DispatchQueue.global(qos: .background).async {
            let img = UIImage(pixelBuffer: frame, withRotation: orientation)
            
            if let detector = self.detector {
                let visionImage = VisionImage(image: img)
                detector.detect(in: visionImage) { (faces, err) in
                    if let error = err {
                        print("\(error)")
                        return
                    }
                    if let faceArray = faces {
                        if faceArray.count > 0 && faceArray[0].smilingProbability > 0.6 {
                            DispatchQueue.main.async {
                                if self.emotion != "smile" {
                                    self.firebase.writeSmileEmotionToDatabase()
                                    print("writing smile to firebase ...")
                                    self.emotion = "smile"
                                }
                                
                            }
                        } else {
                            DispatchQueue.main.async {
                                if self.emotion != "frown" && faceArray.count > 0  {
                                    self.firebase.writeFrownEmotionToDatabase()
                                    self.emotion = "frown"
                                    print("writing frown to firebase ...")
                                }
                            }
                        }
                    }
                }
            }
        }
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
}

extension ViewController: OTPublisherDelegate {
    func publisher(_ publisher: OTPublisherKit, streamCreated stream: OTStream) {
        
    }
    
    func publisher(_ publisher: OTPublisherKit, didFailWithError error: OTError) {
        
    }
}

//image extension code taken from https://github.com/robjperez/SmileDetector
extension UIImage {
    convenience init(pixelBuffer: CVPixelBuffer, withRotation rotation: OTVideoOrientation) {
        let ciImage = CIImage(cvPixelBuffer: pixelBuffer)
        let width = CGFloat(CVPixelBufferGetWidth(pixelBuffer))
        let height = CGFloat(CVPixelBufferGetHeight(pixelBuffer))
        
        let imgRotation: Double = {
            switch (rotation)
            {
            case .up: return 0
            case .right: return .pi / 2
            case .down: return .pi
            case .left: return -.pi / 2
            }
        }()
        
        var tx = CGAffineTransform(translationX: width/2.0, y: height/2.0)
        tx = tx.rotated(by: CGFloat(imgRotation))
        tx = tx.translatedBy(x: -width/2, y: -height/2)
        let transformed = ciImage.transformed(by: tx)
        
        let context = CIContext(options: nil)
        let cgImage = context.createCGImage(transformed, from: CGRect(x: 0, y: 0, width: width, height: height))
        self.init(cgImage: cgImage!, scale: 1.0, orientation: .up)
    }
}

