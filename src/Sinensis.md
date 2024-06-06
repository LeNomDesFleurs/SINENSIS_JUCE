# Sinensis

## 🖼️ UI


## 🛠️ Functionality

-   [ ] lower volume a bit
-   [ ] Midi input if midi is off locks the note 

## ⚙️ Optimization

-   [ ] limit band control widget refresh to certain parameter

## 🪲 Bug

## Other

-   [ ] Get into the [codebase of the tutorial](https://github.com/matkatmusic/SimpleEQ/tree/master/Source)

## Dump

-   [x] envelope not working
-   [x] incorrect management of midi note, the note must be kept to the same voice
-   [-] charte d'épaisseur des traits ?
-   [x] baisser l'attaque minimale
-   [x] pitch wheel
-   [x] attack and decay not working properly
-   [x] ~~changer la courbe de la root frequency~~
-   [x] root freq in midi not working
-   [x] ratio don't update root freq
-   [x] put the feedback saturation in the correct file
-   [x] ~~limit frequency and gain calculation to parameter modification~~
-   [x] note lock don't update frequency bands
-   [x] ~~Rework to resonance gain attenuation to feel the volume increase~~
-   [x] ~~jump in band select ui at beginning~~
-   [x] ~~add dry wet cross fade to processor~~
-   [x] ~~finish dry wet slider~~
-   [-] ~~Q->0 = Dry signal~~
-   [x] ~~Rework poly mode to allow 6x6 filter band~~
-   [x] ~~Only one voice receiveing the correct parameters~~
      `not the good function call, processSinensis / processSample`
-   [x] ~~Lock note~~
-   [x] ~~bug in note lock max value, clip to max note in ui in dsp~~
-   [x] ~~cutoff widget not appearing anymore~~
-   [x] ~~piano widget pour lock note~~
-   [x] ~~widget horizontal / vertical~~
-   [x] ~~Comment set les défauts ? l'ui commence toujours en Odd/even bizarrement~~
-   [-] ~~envoyer pounter vers objet sinensis à l'éditeur, avoir une version locale de l'objet, comparer et copier~~
-   [-] ~~Make ratio and band control inactive in poly mode~~
-   [x] ~~Frequency widget~~
-   [x] ~~Ajouter slider attack/decay~~
-   [x] ~~Y'a un truc bizarre qui se repaint sur le cutoff, je me demande si c'est pas le widget~~
-   [x] ~~Band widget~~
-   [x] ~~use the mode function to set parameters active or not ifelse in the OFF mode~~
-   [x] ~~set slider in the default rotary mode~~
-   [x] ~~Q widget~~
-   [x] ~~Ratio widget~~
-   [x] ~~Buffer le background~~
-   [x] ~~use an enum instead of multiple true false for mode~~
-   [x] ~~Clean the ui header mess~~
-   [x] ~~replacer tout les widgets dans les bounds~~
-   [x] ~~Midi doesn't work~~
-   [x] ~~passer certain slider en horizontal drag ? (y'avais pas un mode avec les deux)~~

## Remplacer les modes par des Enum

Un peu relou pour le lien GUI / APVTS
Pour le type de paramètre, on a `AudioParameterChoice` qui est fait pout ça

```cpp
params.push_back(std::make_unique<AudioParameterChoice> ("BANDMODE", "Band Mode", StringArray { "Low / High", "Odd / Even", "Peak" }, 0));
```

La question c'est quel attachment utiliser dans le `Editor`.

Sur [ce thread](https://forum.juce.com/t/mapping-a-audioparameterchoice-to-a-radio-button-group-sensecheck/18760) ça parle de mapper des radioButtons sur un `AudioParameterChoice` sauf que personne lui a répondu et que je comprends pas bien ce qu'il essaie de faire

Sur [ce thread](https://forum.juce.com/t/combobox-and-audioparameterchoice/31332) du forum ça recommande de faire son propre attachment.
[Ce thread](https://forum.juce.com/t/how-to-implement-radio-buttons-with-parameterattachment-right/44747/3) semble donner un exemple d'attachment custom, j'ai copié son header et cpp pour voir à quoi ça ressemble

## Q widget

-   [x] ~~le peak coupe en haut et les courbe coupe en bas, ajouter des marge en fonction de l'épaisseur~~
-   [x] ~~ajouter couleur~~
-   [ ] essayer des control point plus bas pour que ça pique plus

Assez simple, deux courbe quatratic qui se font face avec les points de contrôle qui monte moins vite que le peak

## Ratio widget

-   [x] ~~calibrer les offsets pour éviter le clipping~~

Placer un centre, ajouter ou soustraire en fonction du ratio
utilisation d'une variable bipolaire pour rendre plus facile la suite du travail

## Band Widget

-   [x] ~~offset, ne se remplit jamais complètement~~
-   [x] ~~réduire l'opacité du background des sliders~~
-   [x] ~~Utiliser un gradient sur les sliders en fonction du type de mode~~
-   [x] ~~plus simple -> couleur en fontion du gain, trouver une interpolation qui fait sens (rouge -> vert fait un dégradé grisouille moche)~~
-   [x] ~~O|E instead of Odd Even and L|H to save size ?~~

Obligé de le faire sur le resize ? J'ai besoin de la valeur du bandMode...

## Frequency cutoff Widget

-   [x] ~~Root Freq~~
-   [ ] Other bands freq and gain
-   [x] ~~Midi Mono Root freq~~
-   [x] ~~Midi Poly Root Freq~~

Pareil, je vais probablement devoir le faire en paint, faire un slider vide sur le visuel ?
Donc potentiellement Band et Frequency seraient juste des slider vide

## Attack / Decay

-   [x] ~~setVisible(false) pour le slider de fréquence~~
-   [x] ~~inspiration slate and ash ?~~
-   [x] ~~ajouter des bord ronds~~
-   [x] ~~placer correctement~~

Go get the drawing function of the variation on hellebore, the rotate transform etc...

## Bug Editor constructor not recognized

Tout à l'heure c'était un problème de fonction pas implémentée
je viens de renommer les extensions d'header, y'a bien moyen que ça vienne de là
Bon visiblement il fallait appeler le header du `editor` dans le cpp de `processor` pour que la classe sois déjà déclarer, j'avoue que ça me semble pas hyper safe ces histoires d'entre header call

## On making Path

Pour dessiner la plupart des choses, il faut

-   instantier un nouveau path
-   le remplir avec des trucs
-   le stroke dans un objet graphic

Dans le cas d'une ligne :

```cpp
juce::Path Lines;
juce::Point<float> point1, point2;
lines.startNewSubPath(point1);
lines.lineTo(point2); //Pas possible de faire les deux d'un coup ??
g.setColout(juce::Colours::black);
g.strokePath(lines, {width, PathStrokeType::curved, PathStrokeType::rounded});
```

Pour faire un rounded Rectangle

```cpp
juce::Path rounded_rectangle;
rounded_rectangle.addRoundedRectangle(
      juce::Rectangle<float>(20, 150, 100, 200), 3);
//stroke ...
```

Beaucoup plus simple :

```cpp
g.fillRoundedRectangle(x, y, width, height, corner size);
```

## Paint background

problème : je ne peux pas draw ma fonction à chaque repaint car elle inclut du random, et puis ce serait pas très malin d'un point de vu processeur, dont besoin de trouver un moyen de paint le background une seul fois.

Avec des borne qui ne s'active que la première fois, ça ne marche pas, je suppose que JUCE flush tout le contenu entre deux frames

[Ce thread](https://forum.juce.com/t/ive-asked-this-before-but-nobody-seemed-to-know-the-answer-i-have-a-component-that-draws-a-vertical-line-playhead-but-it-calls-repaint-on-the-parent-i-just-want-to-draw-the-line/31913/2) mentionne `setBufferToImage()`, [documentation ici](https://docs.juce.com/develop/classComponent.html#af19bbc2186e3297ddd55c328e46c014b).

De ce qu'il me semble comprendre, il faut faire un component et appeler sur lui `setBufferedToImage(true)`.

## update visibility of component depending on mode

Use [setVisible()](https://docs.juce.com/develop/classComponent.html#ac8483af6fe4dc3254e7176df0d8e9f7a)
In the parameter callback.
Radio button problem -> Can't get the right ID, use the apvt for now ?

## flag to set in the cmake /src

`NEEDS_MIDI_INPUT TRUE` aussi une histoire de nombre de canal midi que je n'ai pas encore réussi à résoudre
j'ai essayé `JUCE_VST_NUM_MIDI_INS 1` qui n'a pas fonctionné.

## note lock

-   [x] add buttons
-   [ ] update frequency band UI function
-   [ ] button

## Weird command line header error

```bash
sudo xcode-select -switch /Library/Developer/CommandLineTools
```
