//
//  ExampleVideoCapture.swift
//  smileDetectorAgain
//
//  Created by Grazietta Hof on 2018-07-29.
//  Copyright © 2018 Grazietta Hof. All rights reserved.
//

import Foundation

//entire file taken from https://github.com/opentok/opentok-ios-sdk-samples-swift/blob/master/Custom-Video-Driver/Lets-Build-OTPublisher/ExampleVideoCapture.swift

import OpenTok
import AVFoundation

extension UIApplication {
    func currentDeviceOrientation(cameraPosition pos: AVCaptureDevice.Position) -> OTVideoOrientation {
        var retValue = OTVideoOrientation.up
        DispatchQueue.main.sync {
            retValue = {
                let orientation = statusBarOrientation
                if pos == .front {
                    switch orientation {
                    case .landscapeLeft: return .up
                    case .landscapeRight: return .down
                    case .portrait: return .left
                    case .portraitUpsideDown: return .right
                    case .unknown: return .up
                    }
                } else {
                    switch orientation {
                    case .landscapeLeft: return .down
                    case .landscapeRight: return .up
                    case .portrait: return .left
                    case .portraitUpsideDown: return .right
                    case .unknown: return .up
                    }
                }
            }()
        }
        return retValue
    }
}

extension AVCaptureSession.Preset {
    func dimensionForCapturePreset() -> (width: UInt32, height: UInt32) {
        switch self {
        case AVCaptureSession.Preset.cif352x288:
            return (352, 288)
        case AVCaptureSession.Preset.vga640x480, AVCaptureSession.Preset.high:
            return (640, 480)
        case AVCaptureSession.Preset.low:
            return (192, 144)
        case AVCaptureSession.Preset.medium:
            return (480, 360)
        case AVCaptureSession.Preset.hd1280x720:
            return (1280, 720)
        default:
            return (352, 288)
        }
    }
}

protocol VideoCaptureDelegate {
    func frameCaptured(frame: CVPixelBuffer, orientation: OTVideoOrientation)
}

class ExampleVideoCapture: NSObject, OTVideoCapture {
    var captureSession: AVCaptureSession?
    var videoInput: AVCaptureDeviceInput?
    var videoOutput: AVCaptureVideoDataOutput?
    
    var videoCaptureConsumer: OTVideoCaptureConsumer?
    
    var delegate: VideoCaptureDelegate?
    
    fileprivate var capturePreset: AVCaptureSession.Preset {
        didSet {
            (captureWidth, captureHeight) = capturePreset.dimensionForCapturePreset()
        }
    }
    
    fileprivate var captureWidth: UInt32
    fileprivate var captureHeight: UInt32
    fileprivate var capturing = false
    fileprivate let videoFrame: OTVideoFrame
    
    let captureQueue: DispatchQueue
    
    override init() {
        capturePreset = AVCaptureSession.Preset.vga640x480
        captureQueue = DispatchQueue(label: "com.tokbox.VideoCapture", attributes: [])
        (captureWidth, captureHeight) = capturePreset.dimensionForCapturePreset()
        videoFrame = OTVideoFrame(format: OTVideoFormat(argbWithWidth: captureWidth, height: captureHeight))
    }
    
    // MARK: - AVFoundation functions
    fileprivate func setupAudioVideoSession() throws {
        captureSession = AVCaptureSession()
        captureSession?.beginConfiguration()
        
        captureSession?.sessionPreset = capturePreset
        captureSession?.usesApplicationAudioSession = false
        
        // Configure Camera Input
        guard let device = camera(withPosition: .front)
            else {
                print("Failed to acquire camera device for video")
                return
        }
        
        videoInput = try AVCaptureDeviceInput(device: device)
        guard let videoInput = self.videoInput else {
            print("Error creating input")
            return
        }
        captureSession?.addInput(videoInput)
        
        // Configure Ouput
        videoOutput = AVCaptureVideoDataOutput()
        guard let videoOutput = self.videoOutput else {
            print("Error creating output")
            return
        }
        
        for a in videoOutput.availableVideoPixelFormatTypes {
            print("Codecs: \(a)")
        }
        
        videoOutput.alwaysDiscardsLateVideoFrames = true
        videoOutput.videoSettings = [
            kCVPixelBufferPixelFormatTypeKey as AnyHashable as! String: Int(kCVPixelFormatType_32BGRA)
        ]
        videoOutput.setSampleBufferDelegate(self, queue: captureQueue)
        
        captureSession?.addOutput(videoOutput)
        setFrameRate()
        captureSession?.commitConfiguration()
    }
    
    fileprivate func frameRateRange(forFrameRate fps: Int) -> AVFrameRateRange? {
        return videoInput?.device.activeFormat.videoSupportedFrameRateRanges.filter({ range in
            return range.minFrameRate <= Double(fps) && Double(fps) <= range.maxFrameRate
        }).first
    }
    
    fileprivate func setFrameRate(fps: Int = 20) {
        guard let _ = frameRateRange(forFrameRate: fps)
            else {
                print("Unsupported frameRate \(fps)")
                return
        }
        
        let desiredMinFps = CMTime(value: 1, timescale: CMTimeScale(fps))
        let desiredMaxFps = CMTime(value: 1, timescale: CMTimeScale(fps))
        
        do {
            try videoInput?.device.lockForConfiguration()
            videoInput?.device.activeVideoMinFrameDuration = desiredMinFps
            videoInput?.device.activeVideoMaxFrameDuration = desiredMaxFps
        } catch {
            print("Error setting framerate")
        }
        
    }
    
    fileprivate func camera(withPosition pos: AVCaptureDevice.Position) -> AVCaptureDevice? {
        return AVCaptureDevice.devices(for: AVMediaType.video).filter({ ($0 ).position == pos }).first
    }
    
    fileprivate func updateCaptureFormat(width w: UInt32, height h: UInt32) {
        captureWidth = w
        captureHeight = h
        videoFrame.format = OTVideoFormat(argbWithWidth: w, height: h)
    }
    
    // MARK: - OTVideoCapture protocol
    func initCapture() {
        captureQueue.async {
            do {
                try self.setupAudioVideoSession()
            } catch let error as NSError {
                print("Error configuring AV Session: \(error)")
            }
        }
    }
    
    func start() -> Int32 {
        captureQueue.async {
            self.capturing = true
            self.captureSession?.startRunning()
        }
        return 0
    }
    
    func stop() -> Int32 {
        captureQueue.async {
            self.capturing = false
        }
        return 0
    }
    
    func releaseCapture() {
        let _ = stop()
        videoOutput?.setSampleBufferDelegate(nil, queue: captureQueue)
        captureQueue.sync {
            self.captureSession?.stopRunning()
        }
        captureSession = nil
        videoOutput = nil
        videoInput = nil
        
    }
    
    func isCaptureStarted() -> Bool {
        return capturing && (captureSession != nil)
    }
    
    func captureSettings(_ videoFormat: OTVideoFormat) -> Int32 {
        videoFormat.pixelFormat = .ARGB
        videoFormat.imageWidth = captureWidth
        videoFormat.imageHeight = captureHeight
        return 0
    }
    
    fileprivate func frontFacingCamera() -> AVCaptureDevice? {
        return camera(withPosition: .front)
    }
    
    fileprivate func backFacingCamera() -> AVCaptureDevice? {
        return camera(withPosition: .back)
    }
    
    fileprivate var hasMultipleCameras : Bool {
        return AVCaptureDevice.devices(for: AVMediaType.video).count > 1
    }
    
    func setCameraPosition(_ position: AVCaptureDevice.Position) -> Bool {
        guard let preset = captureSession?.sessionPreset else {
            return false
        }
        
        let newVideoInput: AVCaptureDeviceInput? = {
            do {
                if position == AVCaptureDevice.Position.back {
                    return try AVCaptureDeviceInput.init(device: backFacingCamera()!)
                } else if position == AVCaptureDevice.Position.front {
                    return try AVCaptureDeviceInput.init(device: frontFacingCamera()!)
                } else {
                    return nil
                }
            } catch {
                return nil
            }
        }()
        
        guard let newInput = newVideoInput else {
            return false
        }
        
        var success = true
        
        guard let videoInput = self.videoInput else {
            print("Video Input is not configured")
            return false
        }
        captureQueue.sync {
            captureSession?.beginConfiguration()
            captureSession?.removeInput(videoInput)
            
            if captureSession?.canAddInput(newInput) ?? false {
                captureSession?.addInput(newInput)
                self.videoInput = newInput
            } else {
                success = false
                captureSession?.addInput(videoInput)
            }
            
            captureSession?.commitConfiguration()
        }
        
        if success {
            capturePreset = preset
        }
        
        return success
    }
    
    func toggleCameraPosition() -> Bool {
        guard hasMultipleCameras else {
            return false
        }
        
        if  videoInput?.device.position == .front {
            return setCameraPosition(.back)
        } else {
            return setCameraPosition(.front)
        }
    }
}

extension ExampleVideoCapture: AVCaptureVideoDataOutputSampleBufferDelegate {
    func captureOutput(_ captureOutput: AVCaptureOutput, didDrop sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        print("Dropping frame")
    }
    
    func captureOutput(_ captureOutput: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        if !capturing || videoCaptureConsumer == nil {
            return
        }
        
        guard let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)
            else {
                print("Error acquiring sample buffer")
                return
        }
        
        guard let videoInput = videoInput
            else {
                print("Capturer does not have a valid input")
                return
        }
        
        let time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
        CVPixelBufferLockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: CVOptionFlags(0)))
        
        videoFrame.timestamp = time
        let height = UInt32(CVPixelBufferGetHeight(imageBuffer))
        let width = UInt32(CVPixelBufferGetWidth(imageBuffer))
        
        if width != captureWidth || height != captureHeight {
            updateCaptureFormat(width: width, height: height)
        }
        
        videoFrame.format?.imageWidth = width
        videoFrame.format?.imageHeight = height
        let minFrameDuration = videoInput.device.activeVideoMinFrameDuration
        
        videoFrame.format?.estimatedFramesPerSecond = Double(minFrameDuration.timescale) / Double(minFrameDuration.value)
        videoFrame.format?.estimatedCaptureDelay = 100
        videoFrame.orientation = UIApplication.shared
            .currentDeviceOrientation(cameraPosition: videoInput.device.position)
        
        videoFrame.clearPlanes()
        
        if !CVPixelBufferIsPlanar(imageBuffer) {
            videoFrame.planes?.addPointer(CVPixelBufferGetBaseAddress(imageBuffer))
        } else {
            for idx in 0..<CVPixelBufferGetPlaneCount(imageBuffer) {
                videoFrame.planes?.addPointer(CVPixelBufferGetBaseAddressOfPlane(imageBuffer, idx))
            }
        }
        
        if let delegate = delegate {
            delegate.frameCaptured(frame: imageBuffer, orientation: videoFrame.orientation)
        }
        
        videoCaptureConsumer!.consumeFrame(videoFrame)
        
        CVPixelBufferUnlockBaseAddress(imageBuffer, CVPixelBufferLockFlags(rawValue: CVOptionFlags(0)));
    }
}
