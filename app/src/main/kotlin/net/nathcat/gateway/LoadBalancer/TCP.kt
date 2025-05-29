package net.nathcat.gateway.LoadBalancer

import java.util.concurrent.ExecutorService
import java.net.ServerSocket
import java.net.Socket
import java.net.InetSocketAddress
import java.lang.InterruptedException

/**
 * Runnable which accepts TCP connections on the given thread and
 * submits then to the given execution service
 */
class TCPAcceptor(val port: Int, val parent: Server) : Runnable {
    override public fun run() {
        val server = ServerSocket(port)

        try {
            while (true) {
                val client: Socket = server.accept()
                parent.submitNewForwarder(
                    TCPForwarder(
                            InetSocketAddress(parent.chooseRandomHost(), port),
                            client
                        )
                    )
            }
        } catch (err: InterruptedException) {}
    }
}


/**
 * Forward all incoming bytes to the specified target address
 */
class TCPForwarder(val target: InetSocketAddress, val client: Socket) : Runnable {
    override public fun run() {
        val endpoint = Socket(target.address, target.port)
        val inStream = client.inputStream
        val outStream = endpoint.outputStream

        try {
            while (true) {
                outStream.write(inStream.readAllBytes())
            }
        } catch (err: InterruptedException) {}
    }
}