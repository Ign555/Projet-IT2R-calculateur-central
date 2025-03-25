import sensor, image, lcd
import KPU as kpu

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.run(1)
sensor.set_windowing((224,224))  # Ajout HD pour redimensionner les images du capteur

modele = kpu.load(0x300000)
anchors = [2.0, 2.0, 3.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0, 6.0]
kpu.init_yolo2(modele, 0.48, 0.3, 5, anchors)

while True:
        img = sensor.snapshot()
        Resultats = kpu.run_yolo2(modele, img)
        if Resultats :
            for i in Resultats :
                print(i)
                img.draw_rectangle(i.rect(),color=(0,255,0))
                img.draw_string(i.x(), i.y(), str(int((i.value())*100)) + " % panneau detecte", lcd.GREEN, scale=2)
        lcd.display(img)


