<div align="center">

# ⚡ Reaction Time Game

**Teste seus reflexos: aperte o botão assim que o LED acender — mais rápido você for, melhor!**

[![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=Arduino&logoColor=white)](https://www.arduino.cc/)
[![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)](https://choosealicense.com/licenses/mit/)

</div>

---

## 📋 Descrição

O **Reaction Time Game** é um jogo de reflexos construído com Arduino UNO e display OLED. Um LED acende após um delay aleatório de 1 a 3 segundos — o jogador deve pressionar o botão o mais rápido possível. O tempo de reação é medido com `micros()` para máxima precisão e exibido na tela OLED em milissegundos.

O jogo mantém o **recorde da sessão** e a **média das últimas 5 tentativas**, com navegação completa por telas: menu, gameplay e resultado.

---

## ⚙️ Componentes Utilizados

| Quantidade | Componente | Especificação |
|:---:|---|---|
| 1x | Arduino Uno (ou compatível) | Microcontrolador ATmega328P |
| 1x | Display OLED | SSD1306 128×64 I2C |
| 1x | LED | Qualquer cor — Pino D9 |
| 1x | Resistor para LED | 220Ω (limitador de corrente) |
| 1x | Botão (push button) | Pino D13 |
| — | Jumper wires | Macho-macho |
| 1x | Protoboard | — |

---

## 🔌 Pinagem

```
Arduino Uno
├── D9   → LED + resistor 220Ω → GND
├── D13  → Botão → GND  (INPUT_PULLUP, sem resistor externo)
├── A4   → OLED SDA
└── A5   → OLED SCL
```

---

## 🖼️ Esquemático

![Circuito Real](circuit_images/circuito_real.jpg)

---

## 📦 Bibliotecas Necessárias

Instale pelo **Arduino IDE → Tools → Manage Libraries**:

| Biblioteca | Versão testada |
|---|---|
| Adafruit SSD1306 | 2.5.15 |
| Adafruit GFX Library | — |

---

## 💻 Como Funciona

### Fluxo de Telas

```
[Menu] ──BTN──► [Aguardando] ──delay esgota──► [GO!] ──BTN──► [Resultado]
                     │                                              │
                  BTN antes                                      BTN
                  do LED → [Cedo!] → [Menu]               ──► [Aguardando]
```

| Tela | Conteúdo |
|---|---|
| Menu | Título + Recorde + Média + `[BTN] Iniciar` |
| Aguardando | "Prepare-se..." enquanto conta o delay aleatório |
| GO! | "VAI!" em fonte grande — LED acende neste momento |
| Resultado | Tempo em ms + Recorde + Média das últimas 5 rodadas |
| Cedo! | Exibida 1.2s quando o jogador antecipa o clique |

### Máquina de Estados

```
STATE_MENU → STATE_WAITING → STATE_GO → STATE_RESULT
                 │ (antecipou)               │ (BTN)
              STATE_MENU               STATE_WAITING
```

### Destaques Técnicos

- **`micros()`** para captura precisa do tempo de reação (resolução de 4µs no UNO)
- **Edge detection** no botão (borda HIGH→LOW) — evita duplo disparo ao segurar o botão
- **`randomSeed(analogRead(A0))`** gera seed verdadeiro com ruído do pino flutuante
- **Array circular** de 5 posições para cálculo da média das últimas rodadas
- **`blinkConfirm()`** pisca o LED 3× para confirmar o clique
- Sem `delay()` bloqueante no `loop()` — temporização 100% via `millis()`

---

## 🗂️ Estrutura dos Arquivos

```
ReactionTimeGame/
├── README.md                                          # Esta documentação
├── post_linkedin.txt                                  # Post para LinkedIn
├── sketch_reaction_time_game/
│   └── sketch_reaction_time_game.ino                  # Código completo do projeto
└── circuit_images/
    └── circuito_real.jpg                              # Foto do circuito físico montado
```

---

## 🚀 Como Usar

1. **Monte o circuito** conforme a pinagem acima.
2. **Instale as bibliotecas** Adafruit SSD1306 e Adafruit GFX.
3. **Abra o sketch:** `sketch_reaction_time_game/sketch_reaction_time_game.ino` no Arduino IDE.
4. **Selecione a placa:** `Tools → Board → Arduino Uno`.
5. **Selecione a porta:** `Tools → Port → COMx`.
6. **Faça upload:** `Ctrl+U`.
7. **Pressione o botão** na tela de menu para iniciar.
8. **Aguarde o LED acender** e aperte o botão o mais rápido possível!

---

*Desenvolvido por Felipe Grolla*

