//
//  Firebase.swift
//  SmileDetector
//
//  Created by Grazietta Hof on 2018-07-08.
//

import Firebase

class Firebase {
    
    private let ref: DatabaseReference = {
        return Database.database().reference()
    }()
    
    func writeFrownEmotionToDatabase() {
        self.ref.child("modes").setValue(["mode": "frown"])
    }
    
    func writeSmileEmotionToDatabase() {
        self.ref.child("modes").setValue(["mode": "smile"])
    }
}
