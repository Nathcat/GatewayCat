package net.nathcat.gateway.LoadBalancer

import java.util.concurrent.ExecutorService
import org.json.simple.JSONObject
import org.json.simple.JSONArray
import java.net.InetAddress
import java.util.Random

enum class State {
    RUNNING,
    STOPPED
}

/** Server which handles the routing of all incoming and outgoing packets */
public class Server(private val executor: ExecutorService, private val settings: JSONObject) {
    /**
     * The internal state of the server.
     * Note that if this is set to <code>STOPPED<code>, the server will shutdown all existing threads.
     */
    private var state: State = State.STOPPED
        set(value) {
            state = value
            if (state == State.STOPPED) executor.shutdown()
        }

    /**
     * The current state the server is in
     */
    public val currentState: State
        get() { return state }

    private var hostList: ArrayList<InetAddress> = arrayListOf()
        
    /**
     * Start the server
     */
    public fun start() {
        state = State.RUNNING

        (settings.get("ports")!! as JSONArray).forEach {
            executor.submit(TCPAcceptor((it!! as JSONObject).get("port") as Int, this))
        }

        (settings.get("hosts")!! as JSONArray).forEach {
            hostList.add(InetAddress.getByName(it!! as String))
        }
    }

    /**
     * Stop the server
     */
    public fun stop() {
        state = State.STOPPED
    }

    /**
     * Choose a random host from the current list of hosts
     */
    internal fun chooseRandomHost(): InetAddress {
        return hostList[Random().nextInt(hostList.size)]
    }

    /**
     * Submit a new forwarder into the execution service
     */
    internal fun submitNewForwarder(f: Runnable) {
        executor.submit(f)
    }
}
