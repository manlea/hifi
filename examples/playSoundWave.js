//
//  playSoundWave.js
//  examples
//
//  Created by Ryan Huffman on 05/27/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

Script.include("libraries/globals.js");

var soundClip = new Sound(HIFI_PUBLIC_BUCKET + "sounds/Cocktail%20Party%20Snippets/Walken1.wav");

function playSound() {
    var options = new AudioInjectionOptions();
    var position = MyAvatar.position;
    options.position = position;
    options.volume = 0.5;
    Audio.playSound(soundClip, options);
}

Script.setInterval(playSound, 10000);
