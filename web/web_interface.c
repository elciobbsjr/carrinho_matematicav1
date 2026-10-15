#include "web_interface.h"


static const char pagina_html[] =

"<!DOCTYPE html>\n"
"<html lang=\"pt-BR\">\n"

"<head>\n"

"<meta charset=\"UTF-8\">\n"

"<meta name=\"viewport\" "
"content=\"width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no\">\n"

"<meta name=\"theme-color\" content=\"#38bdf8\">\n"

"<title>Corrida Matemática</title>\n"


"<style>\n"


"*{"
"box-sizing:border-box;"
"margin:0;"
"padding:0;"
"-webkit-tap-highlight-color:transparent;"
"}\n"


"html,body{"
"min-height:100%;"
"}\n"


"body{"
"font-family:'Trebuchet MS','Segoe UI',Arial,sans-serif;"
"background:linear-gradient("
"180deg,"
"#38bdf8 0%,"
"#bae6fd 45%,"
"#f0f9ff 100%"
");"
"display:flex;"
"align-items:center;"
"justify-content:center;"
"padding:16px;"
"color:#0f172a;"
"}\n"


".container{"
"width:100%;"
"max-width:460px;"
"background:#ffffff;"
"border-radius:28px;"
"padding:20px;"
"border:4px solid #ffffff;"
"box-shadow:"
"0 8px 0 #0ea5e9,"
"0 18px 40px rgba(0,0,0,.20);"
"}\n"


".topo{"
"display:flex;"
"align-items:center;"
"justify-content:space-between;"
"gap:10px;"
"margin-bottom:12px;"
"}\n"


"h1{"
"font-size:25px;"
"line-height:1.05;"
"color:#1e3a8a;"
"}\n"


"h1 span{"
"color:#f97316;"
"}\n"


".chip{"
"background:#fef3c7;"
"border:3px solid #fbbf24;"
"color:#92400e;"
"font-weight:900;"
"font-size:17px;"
"border-radius:999px;"
"padding:5px 11px;"
"white-space:nowrap;"
"}\n"


".status{"
"font-size:12px;"
"font-weight:bold;"
"text-align:center;"
"border-radius:999px;"
"padding:6px 12px;"
"margin:0 auto 18px;"
"width:max-content;"
"background:#fef9c3;"
"color:#a16207;"
"}\n"


".status.ok{"
"background:#dcfce7;"
"color:#15803d;"
"}\n"


".status.erro{"
"background:#fee2e2;"
"color:#b91c1c;"
"}\n"


".painel{"
"background:#f8fafc;"
"border:2px solid #e2e8f0;"
"border-radius:20px;"
"padding:14px;"
"margin-bottom:16px;"
"}\n"


".info{"
"display:flex;"
"justify-content:space-between;"
"align-items:center;"
"font-weight:bold;"
"color:#1e40af;"
"font-size:15px;"
"margin-bottom:8px;"
"}\n"


".barra{"
"height:14px;"
"background:#e2e8f0;"
"border-radius:20px;"
"overflow:hidden;"
"}\n"


".barra div{"
"height:100%;"
"width:0;"
"background:linear-gradient("
"90deg,"
"#22c55e,"
"#facc15"
");"
"border-radius:20px;"
"transition:width .5s;"
"}\n"


".instrucao{"
"text-align:center;"
"font-size:15px;"
"font-weight:bold;"
"color:#475569;"
"margin-bottom:8px;"
"}\n"


".pergunta{"
"text-align:center;"
"font-size:46px;"
"font-weight:900;"
"color:#0f172a;"
"background:#fefce8;"
"border:4px dashed #facc15;"
"border-radius:20px;"
"padding:17px 8px;"
"margin-bottom:12px;"
"word-break:break-word;"
"}\n"


".visor{"
"text-align:center;"
"font-size:42px;"
"font-weight:900;"
"color:#2563eb;"
"background:#eff6ff;"
"border:4px solid #93c5fd;"
"border-radius:18px;"
"height:68px;"
"line-height:60px;"
"margin-bottom:12px;"
"}\n"


".visor.vazio{"
"color:#93c5fd;"
"}\n"


".teclado{"
"display:grid;"
"grid-template-columns:repeat(3,1fr);"
"gap:9px;"
"}\n"


".tecla{"
"height:60px;"
"border:0;"
"border-radius:16px;"
"background:#e0f2fe;"
"color:#0c4a6e;"
"font-family:inherit;"
"font-size:29px;"
"font-weight:900;"
"box-shadow:0 5px 0 #7dd3fc;"
"cursor:pointer;"
"}\n"


".tecla:active{"
"transform:translateY(4px);"
"box-shadow:0 1px 0 #7dd3fc;"
"}\n"


".tecla.apaga{"
"background:#fee2e2;"
"box-shadow:0 5px 0 #fca5a5;"
"color:#b91c1c;"
"}\n"


".tecla.ok{"
"background:#22c55e;"
"box-shadow:0 5px 0 #15803d;"
"color:#ffffff;"
"}\n"


".tecla:disabled{"
"opacity:.5;"
"}\n"


".mensagem{"
"min-height:44px;"
"margin-top:13px;"
"border-radius:15px;"
"display:flex;"
"align-items:center;"
"justify-content:center;"
"text-align:center;"
"font-size:15px;"
"font-weight:bold;"
"padding:8px;"
"color:#475569;"
"}\n"


".mensagem.bom{"
"background:#dcfce7;"
"color:#166534;"
"}\n"


".mensagem.mau{"
"background:#ffedd5;"
"color:#9a3412;"
"}\n"


".fim{"
"display:none;"
"text-align:center;"
"padding:25px 10px;"
"}\n"


".fim .icone{"
"font-size:75px;"
"}\n"


".fim h2{"
"font-size:32px;"
"color:#16a34a;"
"margin:10px 0;"
"}\n"


".fim p{"
"font-size:18px;"
"color:#334155;"
"font-weight:bold;"
"line-height:1.4;"
"}\n"


".rodape{"
"text-align:center;"
"margin-top:14px;"
"font-size:11px;"
"color:#94a3b8;"
"}\n"


"@media(max-height:720px){"

".container{"
"padding:14px;"
"}"

".pergunta{"
"font-size:38px;"
"padding:10px;"
"}"

".visor{"
"height:57px;"
"line-height:49px;"
"font-size:34px;"
"}"

".tecla{"
"height:49px;"
"}"

"}\n"


"</style>\n"

"</head>\n"


"<body>\n"


"<main class=\"container\">\n"


"<section id=\"jogo\">\n"


"<div class=\"topo\">\n"

"<h1>"
"Corrida <span>Matemática</span>"
"</h1>\n"


"<div class=\"chip\" id=\"estrelas\">"
"⭐ 0"
"</div>\n"

"</div>\n"


"<div class=\"status\" id=\"status\">"
"Conectando..."
"</div>\n"


"<div class=\"painel\">\n"


"<div class=\"info\">\n"

"<span id=\"questaoNumero\">"
"Carregando..."
"</span>\n"

"<span id=\"porcentagem\">"
"0%"
"</span>\n"

"</div>\n"


"<div class=\"barra\">\n"

"<div id=\"barraProgresso\"></div>\n"

"</div>\n"


"</div>\n"


"<div class=\"instrucao\">"
"Resolva a questão:"
"</div>\n"


"<div class=\"pergunta\" id=\"pergunta\">"
"..."
"</div>\n"


"<div "
"class=\"visor vazio\" "
"id=\"visor\" "
"aria-live=\"polite\">"

"?"

"</div>\n"


"<div class=\"teclado\" id=\"teclado\">\n"


"<button class=\"tecla\" onclick=\"tecla('1')\">1</button>\n"

"<button class=\"tecla\" onclick=\"tecla('2')\">2</button>\n"

"<button class=\"tecla\" onclick=\"tecla('3')\">3</button>\n"


"<button class=\"tecla\" onclick=\"tecla('4')\">4</button>\n"

"<button class=\"tecla\" onclick=\"tecla('5')\">5</button>\n"

"<button class=\"tecla\" onclick=\"tecla('6')\">6</button>\n"


"<button class=\"tecla\" onclick=\"tecla('7')\">7</button>\n"

"<button class=\"tecla\" onclick=\"tecla('8')\">8</button>\n"

"<button class=\"tecla\" onclick=\"tecla('9')\">9</button>\n"


"<button "
"class=\"tecla apaga\" "
"onclick=\"apagar()\">"

"⌫"

"</button>\n"


"<button class=\"tecla\" onclick=\"tecla('0')\">0</button>\n"


"<button "
"class=\"tecla ok\" "
"id=\"botaoResponder\" "
"onclick=\"responder()\">"

"✔"

"</button>\n"


"</div>\n"


"<div "
"class=\"mensagem\" "
"id=\"mensagem\" "
"aria-live=\"polite\">"

"Digite a resposta e pressione ✔"

"</div>\n"


"</section>\n"


"<section id=\"fim\" class=\"fim\">\n"


"<div class=\"icone\">🏁</div>\n"


"<h2>"
"Corrida concluída!"
"</h2>\n"


"<p>"
"Parabéns! Você resolveu todas as questões."
"</p>\n"


"</section>\n"


"<div class=\"rodape\">"
"Corrida Matemática • Raspberry Pi Pico W"
"</div>\n"


"</main>\n"


"<script>\n"


"var valor='';"
"var ocupado=false;"
"var acertos=0;"
"var ultimo=-1;\n"


"function $(id){"
"return document.getElementById(id);"
"}\n"


"function mostrar(){"

"var visor=$('visor');"

"if(valor===''){"

"visor.innerText='?';"
"visor.className='visor vazio';"

"}else{"

"visor.innerText=valor;"
"visor.className='visor';"

"}"

"}\n"


"function tecla(numero){"

"if(ocupado)return;"

"if(valor.length>=6)return;"

"valor+=numero;"

"mostrar();"

"}\n"


"function apagar(){"

"if(ocupado)return;"

"valor=valor.slice(0,-1);"

"mostrar();"

"}\n"


"function conectado(ok){"

"var status=$('status');"

"if(ok){"

"status.className='status ok';"

"status.innerText='● Carrinho conectado';"

"}else{"

"status.className='status erro';"

"status.innerText='● Sem conexão';"

"}"

"}\n"


"function mensagem(texto,tipo){"

"var m=$('mensagem');"

"m.innerText=texto;"

"m.className='mensagem';"

"if(tipo){"
"m.className+=' '+tipo;"
"}"

"}\n"


"async function carregarEstado(){"

"try{"

"var resposta=await fetch("
"'/estado',"
"{cache:'no-store'}"
");"


"if(!resposta.ok){"
"throw new Error('estado');"
"}"


"var dados=await resposta.json();"


"conectado(true);"


"if(dados.finalizada){"

"$('jogo').style.display='none';"

"$('fim').style.display='block';"

"return;"

"}"


"$('questaoNumero').innerText="
"'Questão '+dados.numero+' de '+dados.total;"


"$('porcentagem').innerText="
"dados.progresso+'%';"


"$('barraProgresso').style.width="
"dados.progresso+'%';"


"if(dados.numero!==ultimo){"

"$('pergunta').innerText=dados.pergunta;"

"ultimo=dados.numero;"

"}"


"}catch(erro){"

"conectado(false);"

"setTimeout(carregarEstado,2000);"

"}"

"}\n"


"async function responder(){"

"if(ocupado)return;"


"if(valor===''){"

"mensagem("
"'Digite um número primeiro.',"
"'mau'"
");"

"return;"

"}"


"ocupado=true;"

"$('botaoResponder').disabled=true;"


"mensagem("
"'Verificando...',"
"''"
");"


"try{"


"var resposta=await fetch('/responder',{"

"method:'POST',"

"headers:{"

"'Content-Type':"
"'application/x-www-form-urlencoded'"

"},"

"body:"
"'resposta='+encodeURIComponent(valor)"

"});"


"if(!resposta.ok){"

"throw new Error('http');"

"}"


"var dados=await resposta.json();"


"if(dados.correto){"

"acertos++;"

"$('estrelas').innerText="
"'⭐ '+acertos;"


"mensagem("
"'Muito bem! Resposta correta! 🎉',"
"'bom'"
");"


"valor='';"

"mostrar();"


"await carregarEstado();"


"}else{"


"mensagem("
"'Resposta incorreta. Tente novamente!',"
"'mau'"
");"


"valor='';"

"mostrar();"


"}"


"}catch(erro){"

"conectado(false);"

"mensagem("
"'Não foi possível comunicar com o carrinho.',"
"'mau'"
");"

"}"


"ocupado=false;"

"$('botaoResponder').disabled=false;"

"}\n"


"document.addEventListener("
"'keydown',"
"function(event){"

"if("
"event.key>='0' && "
"event.key<='9'"
"){"

"tecla(event.key);"

"}"

"else if(event.key==='Backspace'){"

"apagar();"

"}"

"else if(event.key==='Enter'){"

"responder();"

"}"

"}"
");\n"


"carregarEstado();\n"


"</script>\n"


"</body>\n"

"</html>\n";


const char *web_interface_get_html(void)
{
    return pagina_html;
}