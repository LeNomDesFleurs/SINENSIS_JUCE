# Sinensis

-   [ ] Frequency widget
-   [ ] Q widget
-   [ ] Ratio widget
-   [ ] Band widget
-   [x] ~~use an enum instead of multiple true false for mode~~
-   [x] ~~Clean the ui header mess~~
-   [ ] replacer tout les widgets dans les bounds

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

Assez simple, deux courbe quatratic qui se font face avec les points de contrôle qui monte moins vite que le peak

## Ratio widget

Placer un centre, ajouter ou soustraire en fonction du ratio

## Band Widget

Obligé de le faire sur le resize ? J'ai besoin de la valeur du bandMode...

## Frequency cutoff Widget

-   [ ] Root Freq
-   [ ] Other bands freq and gain
-   [ ] Midi Mono Root freq
-   [ ] Midi Poly Root Freq

Pareil, je vais probablement devoir le faire en paint, faire un slider vide sur le visuel ?
Donc potentiellement Band et Frequency seraient juste des slider vide

## Bug Editor constructor not recognized

Tout à l'heure c'était un problème de fonction pas implémentée
je viens de renommer les extensions d'header, y'a bien moyen que ça vienne de là
Bon visiblement il fallait appeler le header du `editor` dans le cpp de `processor` pour que la classe sois déjà déclarer, j'avoue que ça me semble pas hyper safe ces histoires d'entre header call
